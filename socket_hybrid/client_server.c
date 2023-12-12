#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "game_p2p.h"

#define CLIENT_IP "127.0.0.1"

void printMessage(char *message) {
    printf("=============================================\n");
    printf("%s\n", message);
    printf("=============================================\n");
}

void printMessageS1(char *message, char *s1) {
    printf("=============================================\n");
    printf("%s%s\n", message, s1);
    printf("=============================================\n");
}

int handle_login(int server_socket, int *socket_port_invite) {
    char buffer[1024];
    char username[50];
    char password[50];
    char socket_port[50];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter port socket invite: ");
    scanf("%s", socket_port);

    // update socket
    *socket_port_invite = atoi(socket_port);

    // data format: LOGIN:username:password
    sprintf(buffer, "LOGIN:%s:%s:%s", username, password, socket_port);

    // Send data to server
    send(server_socket, buffer, strlen(buffer), 0);

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    recv(server_socket, buffer, 1024, 0);
    buffer[10] = '\0';

    printf("Response from server: %s\n", buffer);

    // Check if login successful
    if (strcmp(buffer, "OK") == 0) {
        printMessage("Login successful");
        return 0;
    } else {
        printMessage("Login failed");
        return 1;
    }
}

int handle_register(int server_socket) {
    char buffer[1024];
    char username[50];
    char password[50];
    char confirm_password[50];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Confirm password: ");
    scanf("%s", confirm_password);

    // data format: LOGIN:username:password:confirm_password
    sprintf(buffer, "REGISTER:%s:%s:%s", username, password, confirm_password);

    // Send data to server
    send(server_socket, buffer, strlen(buffer), 0);

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    recv(server_socket, buffer, 1024, 0);

    // Check if register successful
    if (strcmp(buffer, "OK") == 0) {
        printMessage("Register successful");
        return 0;
    } else {
        printMessage("Register failed");
        return 1;
    }
}

int handle_logout(int server_socket) {
    char buffer[1024] = "";

    // data format: LOGOUT
    sprintf(buffer, "LOGOUT:");

    // Send data to server
    send(server_socket, buffer, strlen(buffer), 0);

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    recv(server_socket, buffer, 1024, 0);

    // Check if logout successful
    if (strcmp(buffer, "OK") == 0) {
        printMessage("Logout successful");
        return 0;
    } else {
        printMessageS1("Logout failed: ", buffer);
        return 1;
    }
}

int connect_to_client(char *ip, int port) {
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

void start_game(char* socket_port) {
    int client_socket_2 = connect_to_client(CLIENT_IP, atoi(socket_port));

    while (1)
    {
        user_send(client_socket_2);
        user_recv(client_socket_2);
    }
}

int handle_send_invite(int server_socket) {
    char buffer[1024] = "";
    char username[50] = "";

    int bytes_sent, bytes_received;

    // type username
    printf("Enter username: ");
    scanf("%s", username);

    // data format: SEND_INVITE:username
    sprintf(buffer, "SEND_INVITE:%s", username);

    printf("Buffer: %s\n", buffer);

    // Send data to server
    bytes_sent = send(server_socket, buffer, strlen(buffer), 0);

    printf("Bytes sent: %d\n", bytes_sent);
    if (bytes_sent == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(server_socket, buffer, 1024, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    // set null character
    // buffer[bytes_received] = '\0';
    

    // split message
    char *message_type = strtok(buffer, ":");
    char *socket_port = strtok(NULL, ":");

    // Check if send message successful
    if (strcmp(message_type, "OK") == 0) {
        // close(server_socket);

        printf("Invite successful\n");
        
        start_game(socket_port);

        printMessage("Invite successful");
        return 0;
    } else {
        printMessageS1("Invite failed: ", buffer);
        return 1;
    }
}