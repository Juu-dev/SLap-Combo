#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "action_client.h"

#define SERVER_IP "127.0.0.1" // Thay đổi IP này tùy theo cài đặt của server
#define SERVER_PORT 5000

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char username[50];
    char password[50];
    int conn;

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Định nghĩa địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Kết nối tới server
    conn = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn == -1) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    bool is_running = true;
    while (is_running) {
        // choose option
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("4. Invite\n");
        printf("5. Wait invite\n");
        printf("6. Get list users\n");
        printf("7. Update status\n");
        printf("Choose option: ");
        int option;
        scanf("%d", &option);

        // control flow
        if (option == 1) {
            action_register(server_socket);
        } else if (option == 2) {
            action_login(server_socket);
        } else if (option == 3) {
            is_running = false;
        } else if (option == 4) {
            action_invite(server_socket);
        } else if (option == 5) {
            wait_invite(server_socket);
        } else if (option == 6) {
            action_get_list_users(server_socket);
        } else if (option == 7) {
            action_update_status(server_socket);
        } else {
            printf("Invalid option\n");
            exit(EXIT_FAILURE);
        }
    } 

    // Đóng socket
    close(server_socket);

    return 0;
}
