#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const char login_tag[256] = "LOGIN";
const char register_tag[256] = "REGISTER";
const char challenge_tag[256] = "CHALLENGE";
const char ok_tag[256] = "OK";
const char list_player_tag[256] = "LIST_PLAYER";
const char end_tag[256] = "END";

bool sendDataToClient(int server_socket, char* data) {
    int sendBytes = send(server_socket, data, 256, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi yeu cau cho máy chủ");
        return false;
    }
    return true;
}

bool sendDataToClientConst(int server_socket, const char* data) {
    int sendBytes = send(server_socket, data, 20, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi yeu cau cho máy chủ");
        return false;
    }
    return true;
}

bool recvDataFromClient(int server_socket, char* data) {
    int recvBytes = recv(server_socket, data, 256, 0);
    if (recvBytes < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return false;
    }
    data[recvBytes] = '\0';
    return true;
}

bool serverAvailable(char* response) {
    if (strcmp(response, ok_tag) != 0) {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
        return false;
    }
}

int connect_to_server(const char* server_ip, int server_port) {
    int server_socket;

    // Khởi tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Lỗi khởi tạo socket");
        return -1;
    }

    // Cấu hình địa chỉ và cổng máy chủ
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    // Kết nối tới máy chủ
    if (connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Lỗi kết nối đến máy chủ");
        close(server_socket); // Đóng socket nếu kết nối thất bại
        return -1;
    }
    return server_socket;
}

int login(int server_socket, char username_par[], char password_par[]) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    char username[256], password[256], response[256];
    if (!sendDataToClientConst(server_socket, login_tag)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromClient(server_socket, response)) {
        return -1;
    }

    if (!serverAvailable(response)) {
        return -1;
    }

    strcpy(username, username_par);
    strcpy(password, password_par);

    // Gửi tên đăng nhập và mật khẩu tới máy chủ
    if (!sendDataToClient(server_socket, username)) {
        return -1;
    }
    if (!sendDataToClient(server_socket, password)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromClient(server_socket, response)) {
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

int register_user(int server_socket, char username_par[], char password_par[], char confirm_password_par[]) {
    // Gửi yêu cầu đăng ký tới máy chủ
    char response[256], username[256], password[256], confirm_password[256];

    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendDataToClientConst(server_socket, register_tag)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromClient(server_socket, response)) {
        return -1;
    }

    if (!serverAvailable(response)) {
        return -1;
    }

    // Nhập tên đăng nhập và mật khẩu
    strcpy(username, username_par);
    strcpy(password, password_par);
    strcpy(confirm_password, confirm_password_par);

    // Gửi tên đăng nhập, mật khẩu tới máy chủ
    if (!sendDataToClient(server_socket, username)) {
        return -1;
    }
    if (!sendDataToClient(server_socket, password)) {
        return -1;
    }
    if (!sendDataToClient(server_socket, confirm_password)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromClient(server_socket, response)) {
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

int close_socket(int server_socket) {
    close(server_socket);
    return 0;
}

int send_challenge(int server_socket, int username) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendDataToClientConst(server_socket, challenge_tag)) {
        return -1;
    }

    // Receive response from server
    if (!recvDataFromClient(server_socket, response)) {
        return -1;
    }

    if (strcmp(response, "ACCEPT") == 0) {
        printf("Chap nhan thach dau\n");
        return 0;
    } else if (strcmp(response, "REJECT") == 0) {
        printf("Tu choi thach dau\n");
    } else if (strcmp(response, "TIME_OUT") == 0) {
        printf("Doi thu khong tra loi\n");
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ\n");
    }

    return -1;
}

int response_challenge(int server_socket, int username) {
    // Nhận phản hồi từ máy chủ
    char response[256];
    if (recv(server_socket, response, sizeof(response), 0) < 0) {
        perror("Lỗi nhận phản hồi từ máy chủ");
        return -1;
    }

    return -1;
}

int get_list_player(int server_socket) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendDataToClientConst(server_socket, list_player_tag)) {
        return -1;
    }
   
    bool is_end = false;
    while (!is_end) {
        // Receive response from server
        if (recv(server_socket, response, sizeof(response), 0) < 0) {
            perror("Lỗi nhận phản hồi từ máy chủ");
            return -1;
        }

        if (strcmp(response, end_tag) == 0) {
            is_end = true;
        } else {
            printf("username: %s\n", response);
        }
    }
}