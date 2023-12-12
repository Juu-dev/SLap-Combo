#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "client_server.h"
#include "game_p2p.h"

#define SERVER_IP "127.0.0.1" 
#define SERVER_PORT 5001

int port_socket_invite;

int initialize_client_socket(int port) {
    int client_socket;
    struct sockaddr_in client_addr;

    // Tạo socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Định nghĩa địa chỉ client
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    // Gán địa chỉ cho socket
    if (bind(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(client_socket, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

int connect_to_server(char *ip, int port) {
    int server_socket;
    struct sockaddr_in server_addr;
    int conn;

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Định nghĩa địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Kết nối tới server
    conn = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn == -1) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    return server_socket;
}

void handle_state_idle(int *server_socket_invite, int *client_socket_invite) {
    int client_socket;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[1024];
    int bytes_received;

    // Tạo socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Định nghĩa địa chỉ client
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port_socket_invite);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    // Gán địa chỉ cho socket
    if (bind(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(client_socket, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Client 2 listening on port %d...\n", port_socket_invite);

  
    int new_socket = accept(client_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
    if (new_socket == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Nhận dữ liệu từ server
    bytes_received = recv(new_socket, buffer, 1024, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    // In ra màn hình
    buffer[bytes_received] = '\0';
    printf("Received: %s\n", buffer);

    // update socket
    *server_socket_invite = new_socket;
    *client_socket_invite = client_socket;
}

void wait_user_2_start(int client_socket_invite) {
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int new_socket = accept(client_socket_invite, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
    if (new_socket == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1)
        {
            user_recv(new_socket);
            user_send(new_socket);
        }
}

void handle_send_response(int *server_socket_invite, int *client_socket_invite) {
    char buffer[1024];
    int bytes_sent;

    printf("Enter message: ");
    scanf("%s", buffer);

    // Gửi dữ liệu cho server
    bytes_sent = send(*server_socket_invite, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    if (strcmp(buffer, "OK") == 0) {
        printf("Waiting for game...\n");

        // Waiting invitor
        wait_user_2_start(*client_socket_invite);
        
    } else {
        printf("Response failed\n");
    }

    // Đóng socket
    close(*client_socket_invite);
}

void client_menu(int server_socket) {
    int choice;

    int client_socket_invite, server_socket_invite;

    while (true) {
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Logout\n");
        printf("4. State idle\n");
        printf("4.1. Send Response\n");
        printf("5. Send invite\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                handle_login(server_socket, &port_socket_invite);
                break;
            case 2:
                handle_register(server_socket);
                break;
            case 3:
                handle_logout(server_socket);
                break;
            case 4: 
                handle_state_idle(&server_socket_invite, &client_socket_invite);
                break;
            case 41: 
                handle_send_response(&server_socket_invite, &client_socket_invite);
                break;
            case 5:
                handle_send_invite(server_socket);
                break;
            case 10:
                printf("Goodbye\n");
                return;
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

int main() {
    int server_socket;

    // Kết nối tới server
    server_socket = connect_to_server(SERVER_IP, SERVER_PORT);

    // Hiển thị menu
    client_menu(server_socket);

    // Đóng socket
    close(server_socket);

    return 0;

}