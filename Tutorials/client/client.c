#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Struct data player: username, health, attack, level, status
typedef struct {
    char username[256];
    char password[256];
    int health;
    int attack;
    int level;
    char status[256];
} data_player_struct;

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

int login(int server_socket) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    char request[256] = "LOGIN";
    if (send(server_socket, request, sizeof(request), 0) < 0) {
        perror("Lỗi gửi yêu cầu đăng nhập");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    char response[256];
    if (recv(server_socket, response, sizeof(response), 0) < 0) {
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
    if (send(server_socket, username, sizeof(username), 0) < 0) {
        perror("Lỗi gửi tên đăng nhập");
        return -1;
    }
    if (send(server_socket, password, sizeof(password), 0) < 0) {
        perror("Lỗi gửi mật khẩu");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (recv(server_socket, response, sizeof(response), 0) < 0) {
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

int register_user(int server_socket) {
    // Gửi yêu cầu đăng ký tới máy chủ
    char request[256] = "REGISTER";
    if (send(server_socket, request, strlen(request), 0) < 0) {
        perror("Lỗi gửi yêu cầu đăng ký");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    char response[256];
    if (recv(server_socket, response, sizeof(response), 0) < 0) {
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
    if (send(server_socket, username, sizeof(username), 0) < 0) {
        perror("Lỗi gửi tên đăng ky");
        return -1;
    }
    if (send(server_socket, password, sizeof(password), 0) < 0) {
        perror("Lỗi gửi mật khẩu");
        return -1;
    }
    if (send(server_socket, confirm_password, sizeof(confirm_password), 0) < 0) {
        perror("Lỗi gửi xác nhận mật khẩu");
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (recv(server_socket, response, sizeof(response), 0) < 0) {
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

data_player_struct* get_list_player(int server_socket) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    // data_player_struct player[10];
    data_player_struct* player = malloc(sizeof(data_player_struct) * 10);
    // reset value of player
    for (int i = 0; i < 10; i++) {
        strcpy(player[i].username, "");
        strcpy(player[i].password, "");
        player[i].health = 0;
        player[i].attack = 0;
        player[i].level = 0;
        strcpy(player[i].status, "");
    }
        
    char request[256] = "LIST_PLAYER";
    if (send(server_socket, request, sizeof(request), 0) < 0) {
        perror("Lỗi gửi yêu cầu đăng nhập");
        return NULL;
    }

    bool is_end = false;
    while (!is_end) {
        // Receive data_player from server
        char data_player[256];
        int recvBytes = recv(server_socket, data_player, sizeof(data_player), 0);
        if (recvBytes < 0) {
            perror("Lỗi nhận phản hồi từ máy chủ");
            return NULL;
        }

        if (strcmp(data_player, "END") == 0) {
            printf("Complete: %s\n", data_player);
            is_end = true;
        } else {
            const char field_name[10][256] = {"USERNAME", "PASSWORD", "HEALTH", "ATTACK", "LEVEL", "STATUS"};
            char* token;
            char * delimiter = ";";
            char* data_player_copy = strdup(data_player);
            int index = 0;
            while ((token = strsep(&data_player_copy, delimiter)) != NULL && token[0] != '\0') {
                char* key = strtok(token, "=");
                char* value = strtok(NULL, "=");
                for (int i = 0; i < 6; i++) {
                    if (strcmp(key, field_name[i]) == 0) {
                        if (i == 0) {
                            strcpy(player[index].username, value);
                        } else if (i == 1) {
                            strcpy(player[index].password, value);
                        } else if (i == 2) {
                            player[index].health = atoi(value);
                        } else if (i == 3) {
                            player[index].attack = atoi(value);
                        } else if (i == 4) {
                            player[index].level = atoi(value);
                        } else if (i == 5) {
                            strcpy(player[index].status, value);
                        }
                    }
                }
                index++;
            }
            free(data_player_copy); // Free the allocated memory for data_player_copy

            // print data_player
            for (int i = 0; i < 6; i++) {
                if (i == 0) {
                    printf("username: %s\n", player[i].username);
                } else if (i == 1) {
                    printf("password: %s\n", player[i].password);
                } else if (i == 2) {
                    printf("health: %d\n", player[i].health);
                } else if (i == 3) {
                    printf("attack: %d\n", player[i].attack);
                } else if (i == 4) {
                    printf("level: %d\n", player[i].level);
                } else if (i == 5) {
                    printf("status: %s\n", player[i].status);
                }
            }
        }
    }

    return player;
}

int main(int argc, char* argv[]) {
    // Thực hiện kết nối tới máy chủ
    int server_socket = connect_to_server("127.0.0.1", argc > 1 ? atoi(argv[1]) : 8000);
    if (server_socket < 0) {
        return 1;
    }

    while (1) {
        // Đăng nhập hoặc đăng ký
        int choice;
        printf("1. Đăng nhập\n");
        printf("2. Đăng ký\n");
        printf("3. GET LIST USER\n");
        printf("Nhập lựa chọn: ");
        scanf("%d", &choice);

        if (choice == 1) {
            login(server_socket);
        } else if (choice == 2) {
            register_user(server_socket);
        } else if (choice == 3) {
        data_player_struct* result; 
        result = get_list_player(server_socket);
        } else {
            printf("Lựa chọn không hợp lệ\n");
        }
    }

    // Đóng kết nối
    close(server_socket);

    return 0;
}
