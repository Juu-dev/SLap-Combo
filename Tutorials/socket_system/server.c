#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sqlite3.h>

// #include "database.h"

#define PORT 5000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

sqlite3 *db;

char userLoginUsername[MAX_CLIENTS][256];
int socketUsername[MAX_CLIENTS];
int successfulLoginCount = 0;

void set_player_logged_in(char* username, int socket) {
    if (successfulLoginCount < MAX_CLIENTS) {
        strcpy(userLoginUsername[successfulLoginCount], username);
        socketUsername[successfulLoginCount] = socket;
        successfulLoginCount++;
    }
}

void reset_player_logged_in(int socket) {
    for (int i = 0; i < successfulLoginCount; i++) {
        if (socketUsername[i] == socket) {
            for (int j = i; j < successfulLoginCount - 1; j++) {
                strcpy(userLoginUsername[j], userLoginUsername[j + 1]);
                socketUsername[j] = socketUsername[j + 1];
            }
            successfulLoginCount--;
            break;
        }
    }
}

bool is_player_logged_in(char* username) {
    for (int i = 0; i < successfulLoginCount; i++) {
        if (strcmp(username, userLoginUsername[i]) == 0) {
            return true;
        }
    }
    return false;
}

int initialize_database() {
    // Open database
    int rc = sqlite3_open("user_database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    // Create table
    char *sql = "CREATE TABLE IF NOT EXISTS USERS("  \
                "USERNAME TEXT PRIMARY KEY NOT NULL," \
                "PASSWORD TEXT NOT NULL," \
                "HEALTH TEXT NULL," \
                "ATTACK TEXT NULL," \
                "LEVEL TEXT NULL," \
                "SOCKET TEXT NULL," \
                "STATUS TEXT NULL);";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    } else {
        fprintf(stdout, "USERS Table created successfully\n");
    }
    return 0;
}

void handle_disconnect(int sd, struct sockaddr_in address) {
    reset_player_logged_in(sd);
    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    close(sd);
}

void send_response(int sd, const char *message) {
    send(sd, message, strlen(message), 0);
}

bool verify_login(int sd, char *username, char *password) {
    char sql[1024];
    bool is_exist = false;
    int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        // [Xử lý dữ liệu từ callback]
        printf("Callback function called\n");
        printf("Number of columns: %d\n", argc);

        is_exist = true;
        printf("Username: %s\n", argv[0]);
        printf("Password: %s\n", argv[1]);
        return 1;
    }

    sprintf(sql, "SELECT * FROM USERS WHERE USERNAME = '%s' AND PASSWORD = '%s';", username, password);
    
    printf("==============================================\n");
    printf("SQL: %s\n", sql);
    printf("==============================================\n");
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    if (is_exist == false) {
        // Có lỗi xảy ra
        sqlite3_free(err_msg);
        return false;
    }

    // Nếu xác minh thành công, kiểm tra trạng thái đăng nhập
    if (is_player_logged_in(username)) {
        // Người chơi đã đăng nhập ở nơi khác
        return false;
    }

    // Đánh dấu người chơi đã đăng nhập
    set_player_logged_in(username, sd);

    return true;
}

void handle_login(int sd, char *username, char *password) {
    if (verify_login(sd, username, password)) {
        printf("LOGIN SUCCESSFUL\n");
        send_response(sd, "LOGIN SUCCESSFUL");
    } else {
        printf("LOGIN FAILED\n");
        send_response(sd, "LOGIN FAILED");
    }
}

void register_account(int sd, char *username, char *password, char *confirm_password) {
    if (strcmp(password, confirm_password) != 0) {
        send_response(sd, "Password and confirm password do not match");
        return;
    }

    char sql[1024];
    sprintf(sql, "INSERT INTO USERS (USERNAME, PASSWORD) VALUES ('%s', '%s');", username, password);

    printf("==============================================\n");
    printf("SQL: %s\n", sql);
    printf("==============================================\n");

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("REGISTRATION SUCCESSFUL\n");
        send_response(sd, "REGISTRATION SUCCESSFUL");
    } else {
        // Xử lý lỗi khi thêm người dùng mới vào cơ sở dữ liệu
        fprintf(stderr, "SQL error insert register: %s\n", err_msg);
        sqlite3_free(err_msg);

        printf("REGISTRATION FAILED\n");
        send_response(sd, "REGISTRATION FAILED");
    }
}

// Request A to B: invite:username_B
void send_invite(int sd, char *to_username) {
    // Kiểm tra xem người chơi có tồn tại và đang available không
    if (is_player_logged_in(to_username)) {
        // Gửi lời mời tới client B
        char invite_message[1024];
        int socket_B;
        char username_A[256];
        // check socket from username
        for (int i = 0; i < successfulLoginCount; i++) {
            if (strcmp(to_username, userLoginUsername[i]) == 0) {
                socket_B = socketUsername[i];
                break;
            }
        }
        // check username from socket
        for (int i = 0; i < successfulLoginCount; i++) {
            if (socketUsername[i] == sd) {
                sprintf(username_A, "%s", userLoginUsername[i]);
                break;
            }
        }

        sprintf(invite_message, "invite_from:%s", username_A);
        send(socket_B, invite_message, strlen(invite_message), 0);
    } else {
        // Người chơi không tồn tại hoặc không available, gửi thông báo lỗi về cho client A
        send_response(sd, "Invite failed. Player not available.");
    }
}

// Response B to A: invite_response:username_A:accept
void handle_invite_response(int sd, char *username, char *response) {
    // Kiểm tra xem người chơi có tồn tại và đang available không
    if (is_player_logged_in(username)) {
        // Gửi lời mời tới client B
        char invite_message[1024];
        int socket_A;
        char username_B[256];
        // check socket from username
        for (int i = 0; i < successfulLoginCount; i++) {
            if (strcmp(username, userLoginUsername[i]) == 0) {
                socket_A = socketUsername[i];
                break;
            }
        }
        // check username from socket
        for (int i = 0; i < successfulLoginCount; i++) {
            if (socketUsername[i] == sd) {
                sprintf(username_B, "%s", userLoginUsername[i]);
                break;
            }
        }

        sprintf(invite_message, "invite_response_from:%s:%s", username_B, response);
        send(socket_A, invite_message, strlen(invite_message), 0);
    } else {    
        // Người chơi không tồn tại hoặc không available, gửi thông báo lỗi về cho client A
        send_response(sd, "Invite failed. Player not available.");
    }
}

void handle_start_game(int sd, char *username) {
    // Kiểm tra xem người chơi có tồn tại và đang available không
    if (is_player_logged_in(username)) {
        // Gửi lời mời tới client B
        char invite_message[1024];
        int socket_B;
        // check socket from username
        for (int i = 0; i < successfulLoginCount; i++) {
            if (strcmp(username, userLoginUsername[i]) == 0) {
                socket_B = socketUsername[i];
                break;
            }
        }
        // check username from socket
        strcpy(invite_message, "START");
        send(sd, invite_message, strlen(invite_message), 0);
        send(socket_B, invite_message, strlen(invite_message), 0);
    } else {    
        // Người chơi không tồn tại hoặc không available, gửi thông báo lỗi về cho client A
        send_response(sd, "Invite failed. Player not available.");
    }
}

void handle_client_data(int sd, const char *buffer) {
    char username[50];
    char password[50];
    char confirm_password[50];
    char action[50];

    printf("Client data: %s\n", buffer);

    // Giả sử dữ liệu được gửi theo định dạng "action:username:password:confirm_password"
    if (sscanf(buffer, "%49[^:]:%49[^:]:%49[^:]:%49s", action, username, password, confirm_password) == 4) {
        printf("Action 1: %s\n", action);
        if (strcmp(action, "register") == 0) {
            register_account(sd, username, password, confirm_password);
        } else {
            send_response(sd, "Invalid action");
        }
    } else if (sscanf(buffer, "%49[^:]:%49[^:]:%49s", action, username, password) == 3) {
        printf("Action 2: %s\n", action);
        if (strcmp(action, "login") == 0) {
            handle_login(sd, username, password);
        } else if (strcmp(action, "response_invite") == 0) {
            handle_invite_response(sd, username, password);
        } else {
            send_response(sd, "Invalid action");
        }
    } else if (sscanf(buffer, "%49[^:]:%49[^:]", action, username) == 2) {
        printf("Action 3: %s\n", action);
        if (strcmp(action, "invite") == 0) {
            send_invite(sd, username);
        } else if (strcmp(action, "start_game") == 0) {
            handle_start_game(sd, username);
        } else {
            send_response(sd, "Invalid action");
        }
    } else {
        send_response(sd, "Invalid format");
    }
}

int main() {
    int master_socket, addrlen, new_socket, client_socket[MAX_CLIENTS], max_sd, sd;
    int activity, valread;
    struct sockaddr_in address;
    char buffer[1025];  // Data buffer of 1K

    // Khởi tạo cơ sở dữ liệu
    if (initialize_database() != 0) {
        // Xử lý lỗi nếu không thể khởi tạo cơ sở dữ liệu
        exit(EXIT_FAILURE);
    }

    // Set of socket descriptors
    fd_set readfds;

    // Initialize all client_socket[] to 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // Create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set master socket to allow multiple connections
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Try to specify maximum of 5 pending connections for the master socket
    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // Socket descriptor
            sd = client_socket[i];

            // If valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            // Highest file descriptor number, need it for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Inform user of socket number - used in send and receive commands
            printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Else it's some IO operation on some other socket
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

             if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE - 1)) == 0) {
                    handle_disconnect(sd, address);
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    handle_client_data(sd, buffer);
                }
            }
        }
    }

    return 0;
}
