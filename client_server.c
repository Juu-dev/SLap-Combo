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

int handle_login(char* username, char* password, int server_socket, char* socket_port_invite) {
    char buffer[1024];
    // char username[50];
    // char password[50];
    // char socket_port[50];

    // printf("Enter username: ");
    // scanf("%s", username);
    // printf("Enter password: ");
    // scanf("%s", password);
    // printf("Enter port socket invite: ");
    // scanf("%s", socket_port);

    // update socket
    // *socket_port_invite = atoi(socket_port);

    // data format: LOGIN:username:password
    sprintf(buffer, "LOGIN:%s:%s:%s", username, password, socket_port_invite);

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

int handle_register(int server_socket, char* username, char* password, char* confirm_password) {
    char buffer[1024];
    // char username[50];
    // char password[50];
    // char confirm_password[50];

    // printf("Enter username: ");
    // scanf("%s", username);
    // printf("Enter password: ");
    // scanf("%s", password);
    // printf("Confirm password: ");
    // scanf("%s", confirm_password);

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

void start_game(int socket_port) {
    int client_socket_2 = connect_to_client(CLIENT_IP, socket_port);

    while (1)
    {
        user_send(client_socket_2);
        user_recv(client_socket_2);
    }
}

char* handle_send_invite(int server_socket, char* username) {
    char* buffer = (char*)malloc(1024 * sizeof(char));
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    // char username[50] = "";

    int bytes_sent, bytes_received;

    // type username
    // printf("Enter username: ");
    // scanf("%s", username);

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
    buffer[bytes_received] = '\0';

    return buffer;

    // split message
    // char *message_type = strtok(buffer, ":");
    // char *socket_port = strtok(NULL, ":");



    // // Check if send message successful
    // if (strcmp(message_type, "OK") == 0) {
    //     // close(server_socket);

    //     printf("Invite successful\n");
        
    //     start_game(socket_port);

    //     printMessage("Invite successful");
    //     return 0;
    // } else {
    //     printMessageS1("Invite failed: ", buffer);
    //     return 1;
    // }
}

int handle_get_list_user(int server_socket, char* list_user) {
    char buffer[1024] = "";
    int bytes_sent, bytes_received;

    // data format: GET_LIST_USER
    sprintf(buffer, "GET_LIST_USER:");

    // Send data to server
    bytes_sent = send(server_socket, buffer, strlen(buffer), 0);
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
    buffer[bytes_received] = '\0';

    // Check if get list user successful
    if (strlen(buffer) > 0) {
        printMessage("Get list user successful");
        // copy buffer to list_user
        strcpy(list_user, buffer);
        return 0;
    } else {
        printMessageS1("Get list user failed: ", buffer);
        return 1;
    }
}

int handle_respondent_check(int server_socket) {
    char buffer[1024] = "";
    int bytes_sent, bytes_received;

    // data format: RESPONDENT_CHECK
    sprintf(buffer, "RESPONDENT_CHECK:");

    // Send data to server
    bytes_sent = send(server_socket, buffer, strlen(buffer), 0);
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
    buffer[bytes_received] = '\0';

    printMessage(buffer);

    // format: RESPONDENT_CHECK_RESULT:socket
    char *message_type = strtok(buffer, ":");
    char *socket_ = strtok(NULL, ":");

    // Check if respondent check successful
    if (strcmp(socket_, "NULL") != 0) {
        printMessageS1("Respondent check successful", socket_);
        return 0;
    } else {
        printMessageS1("Respondent check failed: ", buffer);
        return -1;
    }
}

int handle_respondent_response(int server_socket, char* message) {
    char buffer[1024] = "";
    int bytes_sent, bytes_received;

    // data format: RESPONDENT_RESPONSE:message
    sprintf(buffer, "RESPONDENT_RESPONSE:%s", message);

    // Send data to server
    bytes_sent = send(server_socket, buffer, strlen(buffer), 0);
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
    buffer[bytes_received] = '\0';

    // Check if respondent response successful
    if (strcmp(buffer, "Response OK") == 0) {
        printMessage("Respondent response successful");
        return 0;
    } else {
        printMessageS1("Respondent response failed: ", buffer);
        return 1;
    }
}

int handle_petitioner_check(int server_socket, char* username) {
    char buffer[1024] = "";
    int bytes_sent, bytes_received;

    // data format: PETITIONER_CHECK
    sprintf(buffer, "PETITIONER_CHECK:%s", username);

    // Send data to server
    bytes_sent = send(server_socket, buffer, strlen(buffer), 0);
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
    buffer[bytes_received] = '\0';

    printMessage(buffer);

    // format: PETITIONER_CHECK_RESULT:socket
    char *message_type = strtok(buffer, ":");
    char *socket_port = strtok(NULL, ":");

    // Check if petitioner check successful
    if (strcmp(socket_port, "NULL") != 0) {
        char *respondent_status = strtok(NULL, ":");
        if (strcmp(respondent_status, "reject") == 0) {
            printMessageS1("Respondent is busy", socket_port);
            return -1;
        }
        else if (strcmp(respondent_status, "accept") == 0) {
            printMessageS1("Respondent is available", socket_port);
            return 0;
        }
        else {
            printMessageS1("Respondent is not available", socket_port);
            return -1;
        }
    } else {
        printMessageS1("Petitioner check failed: ", buffer);
        return -1;
    }
}

