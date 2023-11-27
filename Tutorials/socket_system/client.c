#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1" // Thay đổi IP này tùy theo cài đặt của server
#define SERVER_PORT 5000

// int action_login(int server_socket, char *username, char *password) {
int action_login(int server_socket) {
    char buffer[1024];

    char username[50];
    char password[50];

    printf("Username: ");
    scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào
    printf("Password: ");
    scanf("%49s", password);

    // Gửi thông tin đăng nhập tới server
    sprintf(buffer, "login:%s:%s", username, password);
    send(server_socket, buffer, strlen(buffer), 0);

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    printf("*****************************************\n");
    printf("Server response: %s\n", buffer);
    printf("*****************************************\n");

    printf("[COMPLETE LOGIN FUNCTION]\n");
    return 0;
}

// int action_register(int server_socket, char *username_par, char *password_par, char *confirm_password_par) {
int action_register(int server_socket) {
    char buffer[1024];
    // Nhập thông tin đăng ký
    char username[50];
    char password[50];
    char confirm_password[50];

    printf("Username: ");
    scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào
    printf("Password: ");
    scanf("%49s", password);
    printf("Confirm password: ");
    scanf("%49s", confirm_password);

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "register:%s:%s:%s", username, password, password);
    send(server_socket, buffer, strlen(buffer), 0);

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    printf("*****************************************\n");
    printf("Server response: %s\n", buffer);
    printf("*****************************************\n");

    printf("[COMPLETE REGISTER FUNCTION]\n");
    return 0;
}

void wait_start_game(int server_socket) {
    char buffer[1024];

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    // split buffer to get username
    
    if (strcmp(buffer, "START") == 0) {
        printf("*****************************************\n");
        printf("Server response: Start game\n");
        printf("*****************************************\n");
    }

    printf("[COMPLETE WAIT START GAME FUNCTION]\n");
}

void action_invite(int server_socket) {
    char buffer[1024];
    char username[50];
    char response[50];

    printf("Username: ");
    scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "invite:%s", username);
    send(server_socket, buffer, strlen(buffer), 0);

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    printf("*****************************************\n");
    printf("Server response: %s\n", buffer);
    printf("*****************************************\n");

    // split buffer to get username, response
    char *token = strtok(buffer, ":");
    token = strtok(NULL, ":");
    strcpy(username, token);
    token = strtok(NULL, ":");
    strcpy(response, token);

    // send start game to server
    if (strcmp(response, "accept") == 0) {
        memset(buffer, 0, sizeof(buffer)); // Xóa buffer
        sprintf(buffer, "start_game:%s", username);
        send(server_socket, buffer, strlen(buffer), 0);
        wait_start_game(server_socket);
    }

    printf("[COMPLETE INVITE FUNCTION]\n");
}

void action_response_invite(int server_socket, char *username) {
    char buffer[1024];
    char response[50];

    printf("Response: ");
    scanf("%49s", response); // Sử dụng %49s để giới hạn kích thước đầu vào

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "response_invite:%s:%s", username, response);
    send(server_socket, buffer, strlen(buffer), 0);

    if (strcmp(response, "accept") == 0) {
        wait_start_game(server_socket);
    }

    printf("[COMPLETE RESPONSE INVITE FUNCTION]\n");
}

void wait_invite(int server_socket) {
    char buffer[1024];
    char username[256];

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    // split buffer to get username
    char *token = strtok(buffer, ":");
    token = strtok(NULL, ":");
    printf("*****************************************\n");
    printf("Server response: Invite from %s\n", token);
    printf("*****************************************\n");

    action_response_invite(server_socket, token);

    printf("[COMPLETE WAIT INVITE FUNCTION]\n");
}


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
        } else {
            printf("Invalid option\n");
            exit(EXIT_FAILURE);
        }
    } 

    // Đóng socket
    close(server_socket);

    return 0;
}
