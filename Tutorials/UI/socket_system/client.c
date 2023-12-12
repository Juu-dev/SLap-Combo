#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define SERVER_IP "127.0.0.1" // Thay đổi IP này tùy theo cài đặt của server
#define SERVER_PORT 5000

char username_wait_invite[256];

// int action_login(int server_socket, char *username, char *password) {
int action_login(int server_socket, char *username, char *password) {
    char buffer[1024];

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

int action_register(int server_socket, char *username, char *password, char *confirm_password) {
    char buffer[1024];

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "register:%s:%s:%s", username, password, confirm_password);
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

int attack_my_turn(int server_socket, int index_struct, int my_socket) {
    char attack[50];
    char buffer[1024];

    printf("Attack: ");
    scanf("%49s", attack); // Sử dụng %49s để giới hạn kích thước đầu vào

    // send to server
    sprintf(buffer, "attack:%d:%d:%s", index_struct, my_socket, attack);
    send(server_socket, buffer, strlen(buffer), 0);
    return 0;
}

void start_game(int server_socket) {
    char buffer[1024];
    bool isLoop = true;

    int index_struct;
    int health_A;
    int health_B;
    int round;
    char turn_for_me[50];
    // is_A_win = -1 => undefined, = 0 => fail, = 1 => victory
    int is_A_win;
    int my_socket;

    printf("////////////////////////////////////////////\n");
    printf("LET'S PLAY GAME\n");

    while (isLoop) {
        // Nhận phản hồi từ server
        memset(buffer, 0, sizeof(buffer)); // Xóa buffer
        recv(server_socket, buffer, sizeof(buffer) - 1, 0);

        printf("*****************************************\n");
        printf("Server response playing game: %s\n", buffer);
        printf("*****************************************\n");

        // split to elements
        if (sscanf(buffer, "%d:%d:%d:%d:%49[^:]:%d:%d", &index_struct, &health_A, &health_B, &round, turn_for_me, &is_A_win, &my_socket) == 7) {
            printf("index_struct: %d\n", index_struct);
            printf("health_A: %d\n", health_A);
            printf("health_B: %d\n", health_B);
            printf("round: %d\n", round);
            printf("turn_for_me: %s\n", turn_for_me);
            printf("is_A_win: %d\n", is_A_win);
            printf("my_socket: %d\n", my_socket);

            if (is_A_win == 1) {
                isLoop = false;
                if (strcmp(turn_for_me, "true") == 0) {
                    printf("YOU LOSE\n");
                } else {
                    printf("YOU WIN\n");
                }
            }

            if (strcmp(turn_for_me, "true") == 0 && isLoop) {
                attack_my_turn(server_socket, index_struct, my_socket);
            }
        }
    }
}

void wait_start_game(int server_socket) {
    char buffer[6];
    printf("Waiting for start game\n");

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    // split buffer to get username
    
    printf("Waiting for start game: %s\n", buffer);

    if (strcmp(buffer, "START") == 0) {
        printf("*****************************************\n");
        printf("Server response: Start game\n");
        // start_game(server_socket);
        printf("*****************************************\n");
    }
}

int action_invite(int server_socket, char* username) {
    char buffer[1024];
    char username_token[50];
    char response[50];

    // printf("Username: ");
    // scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào

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
    if (sscanf(buffer, "%49[^:]:%49[^:]", username_token, response) == 2) {
        printf("Username: %s\n", username_token);
        printf("Response: %s\n", response);
    }

    // send start game to server
    if (strcmp(response, "accept") == 0) {
        memset(buffer, 0, sizeof(buffer)); // Xóa buffer
        sprintf(buffer, "start_game:%s", username);
        send(server_socket, buffer, strlen(buffer), 0);
        wait_start_game(server_socket);
    }

    printf("[COMPLETE INVITE FUNCTION]\n");
    return 0;
}

int action_response_invite(int server_socket, char *username, char *response) {
    char buffer[1024];

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "response_invite:%s:%s", username, response);
    send(server_socket, buffer, strlen(buffer), 0);

    if (strcmp(response, "accept") == 0) {
        printf("***************OK***************\n");
        wait_start_game(server_socket);
    }

    printf("[COMPLETE RESPONSE INVITE FUNCTION]\n");
    return 0;
}

int action_wait_invite(char *username, int server_socket) {
    fd_set readfds;
    struct timeval tv;
    int retval;
    
    // Khởi tạo một tập hợp chứa một socket
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);

    // Đặt thời gian chờ
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(server_socket + 1, &readfds, NULL, NULL, &tv);

    if (retval == -1) {
        perror("select()");
        return 1;
    } else if (retval) {
        printf("Data is available now.\n");
        // Đọc dữ liệu ở đây
        char buffer[1024];
        char username_token[50];
        char response[50];

        // Nhận phản hồi từ server
        memset(buffer, 0, sizeof(buffer)); // Xóa buffer
        recv(server_socket, buffer, sizeof(buffer) - 1, 0);
        printf("*****************************************\n");
        printf("Server response: %s\n", buffer);
        printf("*****************************************\n");
    } else {
        printf("No data within five seconds.\n");
    }

    // printf("*****************************************\n");
    // printf("Server response: Invite from %s\n", token);
    // printf("*****************************************\n");

    printf("[COMPLETE WAIT INVITE FUNCTION]\n");
    return 0;
}

int action_get_list_player(int server_socket, char *list_player) {
    char buffer[256];

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "get_list_users");
    send(server_socket, buffer, strlen(buffer), 0);

    // // Nhận phản hồi từ server
    // memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    // recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    strcpy(list_player, buffer);

    printf("*****************************************\n");
    printf("Server response: %s\n", buffer);
    printf("*****************************************\n");

    printf("[COMPLETE GET LIST USERS FUNCTION]\n");
    return 0;
}

int action_update_status(int server_socket, char *status) {
    char buffer[256];
    // char status[50];

    // printf("Status: ");
    // scanf("%49s", status);

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "update_status: %s", status);
    send(server_socket, buffer, strlen(buffer), 0);

    printf("[COMPLETE UPDATE STATUS FUNCTION]\n");
    return 0;
}

void close_client_socket(int server_socket) {
    close(server_socket);
}

int connect_to_server(char *ip, int port) {
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
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Kết nối tới server
    conn = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn == -1) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    return server_socket;
}


int main() {}