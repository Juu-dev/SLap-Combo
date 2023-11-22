#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int connect_to_server(const char* server_ip, int server_port) {
    int client_socket;

    // Khởi tạo socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Lỗi khởi tạo socket");
        return -1;
    }

    // Cấu hình địa chỉ và cổng máy chủ
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    // Kết nối tới máy chủ
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Lỗi kết nối đến máy chủ");
        close(client_socket); // Đóng socket nếu kết nối thất bại
        return -1;
    }

    return client_socket;
}

int login(int client_socket, char username_par[], char password_par[]) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    char request[256] = "LOGIN";
    if (send(client_socket, request, sizeof(request), 0) < 0) {
        perror("Lỗi gửi yêu cầu đăng nhập");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    char response[256];
    if (recv(client_socket, response, sizeof(response), 0) < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return -1;
    }

    if (strcmp(response, "OK") != 0) {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }

    // Nhập tên đăng nhập và mật khẩu
    char username[256];
    char password[256];
    
    strcpy(username, username_par);
    strcpy(password, password_par);

    // printf("Tên đăng nhập: ");
    // scanf("%s", username);
    // printf("Mật khẩu: ");
    // scanf("%s", password);

    // Gửi tên đăng nhập và mật khẩu tới máy chủ
    if (send(client_socket, username, sizeof(username), 0) < 0) {
        perror("Lỗi gửi tên đăng nhập");
        return -1;
    }
    if (send(client_socket, password, sizeof(password), 0) < 0) {
        perror("Lỗi gửi mật khẩu");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (recv(client_socket, response, sizeof(response), 0) < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return -1;
    }

    // Xử lý phản hồi từ máy chủ
    if (strcmp(response, "LOGIN_SUCCESS") == 0) {
        printf("Đăng nhập thành công\n");
        return 0;
    } else if (strcmp(response, "LOGIN_FAIL") == 0) {
        printf("Đăng nhập thất bại\n");
        return -1;
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }
}

int register_user(int client_socket, char username_par[], char password_par[], char confirm_password_par[]) {
    // Gửi yêu cầu đăng ký tới máy chủ
    char request[256] = "REGISTER";
    if (send(client_socket, request, strlen(request), 0) < 0) {
        perror("Lỗi gửi yêu cầu đăng ký");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    char response[256];
    if (recv(client_socket, response, sizeof(response), 0) < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return -1;
    }

    if (strcmp(response, "OK") != 0) {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }

    // Nhập tên đăng nhập và mật khẩu
    char username[256];
    char password[256];
    char confirm_password[256];

    strcpy(username, username_par);
    strcpy(password, password_par);
    strcpy(confirm_password, confirm_password_par);

    // printf("Tên đăng nhập: ");
    // scanf("%s", username);
    // printf("Mật khẩu: ");
    // scanf("%s", password);
    // printf("Xác nhận mật khẩu: ");
    // scanf("%s", confirm_password);

    // Gửi tên đăng nhập, mật khẩu tới máy chủ
    if (send(client_socket, username, sizeof(username), 0) < 0) {
        perror("Lỗi gửi tên đăng ky");
        return -1;
    }
    if (send(client_socket, password, sizeof(password), 0) < 0) {
        perror("Lỗi gửi mật khẩu");
        return -1;
    }
    if (send(client_socket, confirm_password, sizeof(confirm_password), 0) < 0) {
        perror("Lỗi gửi xác nhận mật khẩu");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (recv(client_socket, response, sizeof(response), 0) < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return -1;
    }

    // Xử lý phản hồi từ máy chủ
    if (strcmp(response, "REGISTER_SUCCESS") == 0) {
        printf("Đăng ký thành công\n");
        return 0;
    } else if (strcmp(response, "REGISTER_FAIL") == 0) {
        printf("Đăng ký thất bại\n");
        return -1;
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }
}

int close_socket(int client_socket) {
    close(client_socket);
    return 0;
}