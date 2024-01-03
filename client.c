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

void close_socket(int client_socket) {
    printf("Close socket %d\n", client_socket);
    close(client_socket);
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


// void client_menu(int server_socket) {
//     int choice;

//     int client_socket_invite, server_socket_invite;

//     while (true) {
//         printf("1. Login\n");
//         printf("2. Register\n");
//         printf("3. Logout\n");
//         printf("4. State idle\n");
//         printf("4.1. Send Response\n");
//         printf("5. Send invite\n");
//         printf("10. Exit\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);

//         switch (choice) {
//             case 4: 
//                 handle_state_idle(&server_socket_invite, &client_socket_invite);
//                 break;
//             case 41: 
//                 handle_send_response(&server_socket_invite, &client_socket_invite);
//                 break;
//             case 10:
//                 printf("Goodbye\n");
//                 return;
//             default:
//                 printf("Invalid choice\n");
//                 break;
//         }
//     }
// }

// int main() {
//     int server_socket;

//     // Kết nối tới server
//     server_socket = connect_to_server(SERVER_IP, SERVER_PORT);

//     // Hiển thị menu
//     client_menu(server_socket);

//     // Đóng socket
//     close(server_socket);

//     return 0;

// }