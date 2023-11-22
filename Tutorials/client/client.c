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

int login(int client_socket) {
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
    printf("Tên đăng nhập: ");
    scanf("%s", username);
    printf("Mật khẩu: ");
    scanf("%s", password);

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
    } else if (strcmp(response, "LOGIN_FAILURE") == 0) {
        printf("Đăng nhập thất bại\n");
        return -1;
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }
}

int register_user(int client_socket) {
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
    printf("Tên đăng nhập: ");
    scanf("%s", username);
    printf("Mật khẩu: ");
    scanf("%s", password);
    printf("Xác nhận mật khẩu: ");
    scanf("%s", confirm_password);

    // Gửi tên đăng nhập, mật khẩu tới máy chủ
    if (send(client_socket, username, sizeof(username), 0) < 0) {
        perror("Lỗi gửi tên đăng nhập");
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
    } else if (strcmp(response, "REGISTER_FAILURE") == 0) {
        printf("Đăng ký thất bại\n");
        return -1;
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return -1;
    }
}

int main(int argc, char* argv[]) {
    // Thực hiện kết nối tới máy chủ
    int client_socket = connect_to_server("127.0.0.1", argc > 1 ? atoi(argv[1]) : 8000);
    if (client_socket < 0) {
        return 1;
    }

    while (1) {
        // Đăng nhập hoặc đăng ký
        int choice;
        printf("1. Đăng nhập\n");
        printf("2. Đăng ký\n");
        printf("Nhập lựa chọn: ");
        scanf("%d", &choice);

        if (choice == 1) {
            if (login(client_socket) < 0) {
                return 1;
            }
        } else if (choice == 2) {
            if (register_user(client_socket) < 0) {
                return 1;
            }
        } else {
            printf("Lựa chọn không hợp lệ\n");
            return 1;
        }
    }

    // Đóng kết nối
    close(client_socket);

    return 0;
}
