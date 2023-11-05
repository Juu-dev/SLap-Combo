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

    // Gửi và nhận dữ liệu với máy chủ ở đây
    char data_to_send[] = "Hello, server!";
    int bytes_sent = send(client_socket, data_to_send, strlen(data_to_send), 0);
    if (bytes_sent < 0) {
        perror("Lỗi gửi dữ liệu");
        close(client_socket); // Đóng socket nếu gửi dữ liệu thất bại
        return -1;
    }

    char received_data[1024];
    int bytes_received = recv(client_socket, received_data, sizeof(received_data), 0);
    if (bytes_received < 0) {
        perror("Lỗi nhận dữ liệu");
        close(client_socket); // Đóng socket nếu nhận dữ liệu thất bại
        return -1;
    }

    received_data[bytes_received] = '\0'; // Đảm bảo chuỗi kết thúc bằng null
    printf("Dữ liệu nhận được từ máy chủ: %s\n", received_data);

    // Đóng kết nối
    close(client_socket);

    return 0;
}

int main() {
    // Thực hiện kết nối tới máy chủ
    if (connect_to_server("127.0.0.1", 12345) < 0) {
        return 1;
    }

    return 0;
}
