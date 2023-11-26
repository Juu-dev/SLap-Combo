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

const char login_tag[256] = "LOGIN";
const char register_tag[256] = "REGISTER";
const char challenge_tag[256] = "REQUEST_CHALLENGE";
const char look_req_chal[256] = "LOOK_REQUEST_CHALLENGE";
const char res_chal[256] = "RESPONSE_CHALLENGE";
const char look_res_chal[256] = "LOOK_RESPONSE_CHALLENGE";
const char start_game_tag[256] = "START_GAME";
const char ok_tag[256] = "OK";
const char list_player_tag[256] = "LIST_PLAYER";
const char end_tag[256] = "END";

bool sendDataToServer(int server_socket, char* data) {
    int sendBytes = send(server_socket, data, 256, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi yeu cau cho máy chủ");
        return false;
    }
    return true;
}

bool sendTagFunction(int server_socket, const char* data) {
    int sendBytes = send(server_socket, data, 20, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi yeu cau cho máy chủ");
        return false;
    }
    return true;
}

bool recvDataFromServer(int server_socket, char* data) {
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
        printf("Phản hồi không hợp lệ từ máy chủ, serverAvailable error\n");
        return false;
    }
    return true;
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

int login(int server_socket) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    char username[256], password[256], response[256];
    if (!sendTagFunction(server_socket, login_tag)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    printf("Response: %s\n", response);

    if (!serverAvailable(response)) {
        return -1;
    }



    // Nhập tên đăng nhập và mật khẩu
    printf("Tên đăng nhập: ");
    scanf("%s", username);
    printf("Mật khẩu: ");
    scanf("%s", password);

     // Gửi tên đăng nhập và mật khẩu tới máy chủ
    if (!sendDataToServer(server_socket, username)) {
        return -1;
    }
    if (!sendDataToServer(server_socket, password)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromServer(server_socket, response)) {
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
        printf("Phản hồi không hợp lệ từ máy chủ, login error\n");
        return -1;
    }
}

int register_user(int server_socket) {
    // Gửi yêu cầu đăng ký tới máy chủ
     char response[256], username[256], password[256], confirm_password[256];

    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendTagFunction(server_socket, register_tag)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    if (!serverAvailable(response)) {
        return -1;
    }

    // Nhập tên đăng nhập và mật khẩu
    printf("Tên đăng nhập: ");
    scanf("%s", username);
    printf("Mật khẩu: ");
    scanf("%s", password);
    printf("Xác nhận mật khẩu: ");
    scanf("%s", confirm_password);

    // Gửi tên đăng nhập, mật khẩu tới máy chủ
    if (!sendDataToServer(server_socket, username)) {
        return -1;
    }
    if (!sendDataToServer(server_socket, password)) {
        return -1;
    }
    if (!sendDataToServer(server_socket, confirm_password)) {
        return -1;
    }

    // Nhận phản hồi từ máy chủ
    if (!recvDataFromServer(server_socket, response)) {
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
        printf("Phản hồi không hợp lệ từ máy chủ, register user error\n");
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
        
    if (!sendTagFunction(server_socket, list_player_tag)) {
        return NULL;
    }

    bool is_end = false;
    while (!is_end) {
        // Receive data_player from server
        char data_player[256];
        if (!recvDataFromServer(server_socket, data_player)) {
            return NULL;
        }

        if (strcmp(data_player, end_tag) == 0) {
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

int request_challenge(int server_socket, int username_INT) {
    char response[256];
    char username[256] = "thong_2";
    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendTagFunction(server_socket, challenge_tag)) {
        return -1;
    }

    // Receive OK from server
    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    if (!serverAvailable(response)) {
        return -1;
    }

    // SEND USERNAME TO SERVER
    if (!sendDataToServer(server_socket, username)) {
        return -1;
    }

    // wait for response from server
    // Receive response from server
    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    printf("Response request challenge: %s\n", response);

    if (strcmp(response, "ACCEPT") == 0) {
        printf("Chap nhan thach dau\n");
        return 0;
    } else if (strcmp(response, "REJECT") == 0) {
        printf("Tu choi thach dau\n");
    } else if (strcmp(response, "TIME_OUT") == 0) {
        printf("Doi thu khong tra loi\n");
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ, request challenge error\n");
    }

    return -1;
}

int look_request_challenge(int server_socket) {
    // Nhận phản hồi từ máy chủ
    char response[256];
    
    if (!sendTagFunction(server_socket, look_req_chal)) {
        return -1;
    }

    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    printf("Look Request Challege: %s\n", response);

    return 0;
}

int response_challenge(int server_socket, char* response) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendTagFunction(server_socket, res_chal)) {
        return -1;
    }

    if (!sendDataToServer(server_socket, response)) {
        return -1;
    }

    // ==============================================
    // Nhận phản hồi từ máy chủ
    char server_tag[256];
    if (!recvDataFromServer(server_socket, server_tag)) {
        return -1;
    }

    if (strcmp(server_tag, "START_GAME") == 0) {
        printf("Bat dau game\n");
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ, start game error\n");
    }

    return 0;
}

int start_game(int server_socket) {
    // Gửi yêu cầu đăng nhập tới máy chủ
    if (!sendTagFunction(server_socket, start_game_tag)) {
        return -1;
    }

    // ==============================================
    // Nhận phản hồi từ máy chủ
    char response[256];
    if (!recvDataFromServer(server_socket, response)) {
        return -1;
    }

    if (strcmp(response, "START_GAME") == 0) {
        printf("Bat dau game\n");
    } else {
        printf("Phản hồi không hợp lệ từ máy chủ, start game error\n");
    }

    return 0;
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
        printf("\n");
        printf("=====================================\n");
        printf("1. Đăng nhập\n");
        printf("2. Đăng ký\n");
        printf("3. GET LIST USER\n");
        printf("4. CHALLENGE\n");
        printf("5. LOOK REQUEST CHALLENGE\n");
        printf("6. RESPONSE CHALLENGE\n");
        printf("7. START GAME\n");
        printf("Nhập lựa chọn: ");
        scanf("%d", &choice);

        if (choice == 1) {
            login(server_socket);
        } else if (choice == 2) {
            register_user(server_socket);
        } else if (choice == 3) {
        data_player_struct* result; 
        result = get_list_player(server_socket);
        } else if (choice == 4) {
            request_challenge(server_socket, 1);
        } else if (choice == 5) {
            look_request_challenge(server_socket);
        } else if (choice == 6) {
            response_challenge(server_socket, "ACCEPT");
        } else if (choice == 7) {
            start_game(server_socket);
        } else {
            printf("Lựa chọn không hợp lệ\n");
        }
    }

    // Đóng kết nối
    close(server_socket);

    return 0;
}
