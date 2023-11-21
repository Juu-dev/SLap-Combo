#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 10

int server_socket;
int client_sockets[MAX_CLIENTS];
pthread_t client_threads[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* client_handler(void* client_socket_ptr) {
    int client_socket = *((int*)client_socket_ptr);
    free(client_socket_ptr);

    char received_data[1024];
    int bytes_received = recv(client_socket, received_data, sizeof(received_data), 0);

    if (bytes_received <= 0) {
        close(client_socket);
        pthread_exit(NULL);
    }

    received_data[bytes_received] = '\0';
    printf("Received data from client: %s\n", received_data);

    // Phân tích thông tin đăng nhập (ví dụ: tách tên người dùng và mật khẩu)
    char username[256];
    char password[256];
    if (sscanf(received_data, "%255s %255s", username, password) == 2) {
        // Thực hiện đăng nhập
        if (login_user(username, password) == 0) {
            printf("Đăng nhập thành công!\n");
            const char* response = "Login successful"; 
            send(client_socket, response, strlen(response), 0); 
        } else {
            printf("Đăng nhập thất bại!\n");
            const char* response = "Login failed"; 
            send(client_socket, response, strlen(response), 0); 
        }
    }

    close(client_socket);
    pthread_exit(NULL);
}

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
    if (listen(server_socket, 10) < 0) { 
        perror("Lỗi lắng nghe kết nối");
        return -1;
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        int i;
        pthread_t thread;

        pthread_mutex_lock(&mutex);
        for (i = 0; i < MAX_CLIENTS && client_sockets[i] != -1; i++) {
        }
        if (i < MAX_CLIENTS) {
            client_sockets[i] = client_socket;
            int* client_socket_ptr = malloc(sizeof(int));
            *client_socket_ptr = client_socket;
            if (pthread_create(&thread, NULL, client_handler, client_socket_ptr) != 0) {
                perror("Error creating thread");
            } else {
                client_threads[i] = thread;
            }
        } else {
            close(client_socket);
        }
        pthread_mutex_unlock(&mutex);
    }

    close(server_socket);
    return 0;
}

int main() {
    if (start_server(12345) < 0) {
        return 1;
    }

    printf("Waiting for connections...\n");

    while (1) {
        // Keep the server running
    }

    return 0;
}
