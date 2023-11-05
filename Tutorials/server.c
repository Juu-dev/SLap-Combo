#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int server_socket, client_socket;

int start_server(int port) {
    // Khởi tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Lỗi khởi tạo socket");
        return -1;
    }

    // Cấu hình địa chỉ máy chủ
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Gắn địa chỉ và cổng vào socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Lỗi gắn địa chỉ và cổng vào socket");
        return -1;
    }

    // Lắng nghe kết nối từ client
    if (listen(server_socket, 5) < 0) {
        perror("Lỗi lắng nghe kết nối");
        return -1;
    }

    return 0;
}

int accept_connection() {
    // Chấp nhận kết nối từ client
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("Lỗi chấp nhận kết nối");
        return -1;
    }
    return 0;
}

int send_data(const char* data, int data_length) {
    int bytes_sent = send(client_socket, data, data_length, 0);
    if (bytes_sent < 0) {
        perror("Lỗi gửi dữ liệu");
        return -1;
    }
    return bytes_sent;
}

int receive_data(char* buffer, int buffer_size) {
    int bytes_received = recv(client_socket, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        perror("Lỗi nhận dữ liệu");
        return -1;
    }
    return bytes_received;
}

void close_connection() {
    close(client_socket);
    close(server_socket);
}

int main() {
    // Khởi tạo máy chủ
    if (start_server(12345) < 0) {
        return 1;
    }

    printf("Đang chờ kết nối từ client...\n");

    // Chấp nhận kết nối từ client
    if (accept_connection() < 0) {
        return 1;
    }

    // Gửi và nhận dữ liệu với client ở đây
    char data_to_send[] = "Hello, client!";
    send_data(data_to_send, strlen(data_to_send));

    char received_data[1024];
    int bytes_received = receive_data(received_data, sizeof(received_data));
    if (bytes_received > 0) {
        received_data[bytes_received] = '\0'; // Đảm bảo chuỗi kết thúc bằng null
        printf("Dữ liệu nhận được từ client: %s\n", received_data);
    }

    // Đóng kết nối
    close_connection();

    return 0;
}
