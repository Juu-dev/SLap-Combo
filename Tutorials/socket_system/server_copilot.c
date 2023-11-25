#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sqlite3.h>
#include <errno.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

sqlite3 *db;
int server_socket;
int client_sockets[MAX_CLIENTS];

bool sendDataToClient(int client_socket, char* data) {
    int sendBytes = send(client_socket, data, BUFFER_SIZE, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi phản hồi cho máy khách");
        return false;
    }
    return true;
}

bool recvDataFromClient(int client_socket, char* data) {
    int recvBytes = recv(client_socket, data, BUFFER_SIZE, 0);
    if (recvBytes < 0) {
        perror("Lỗi nhận phản hồi từ máy khách");
        return false;
    }
    data[recvBytes] = '\0';
    return true;
}

// Các hàm xử lý đăng nhập, đăng ký, liệt kê người chơi, vv.
// handle_login(client_socket, buffer), handle_register(client_socket, buffer), ...

int initialize_database() {
    // Khởi tạo và cấu hình cơ sở dữ liệu
    // sqlite3_open(), sqlite3_exec()...
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


void handle_login(int client_socket, char* data) {
    // send OK to client
    char start[256] = "OK", username[256], password[256], sql[1024];
    int user_exists = 0, rc;
    int callback(void *data, int argc, char **argv, char **azColName) {
        user_exists = 1;
        return 0;
    }

    // send(client_socket, start, sizeof(start), 0);
    if (!sendDataToClient(client_socket, start)) return;
    // TODO: Implement login logic
    // Example:
    printf("Handling login.\n");
    // receive data login from client

    // if (!recv(client_socket, username, 256, 0)) return;
    // if (!recv(client_socket, password, 256, 0)) return;
    if (!recvDataFromClient(client_socket, username)) return;
    if (!recvDataFromClient(client_socket, password)) return;

    // check if user already logged in
    pthread_mutex_lock(&mutex);
    bool userAlreadyLoggedIn = false;
    for (int i = 0; i < successfulLoginCount; i++) {
        if (strcmp(username, userLoginUsername[i]) == 0) {
            userAlreadyLoggedIn = true;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);


    // Check if username exists in database
    if (!userAlreadyLoggedIn) {
        sprintf(sql, "SELECT * FROM USERS WHERE USERNAME='%s' AND PASSWORD='%s';", username, password);
        char *err_msg = 0;
        rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    }
    
    char response[256];
    if (rc == SQLITE_OK && user_exists == 1) {
        // Lock the mutex before updating the global data structure
        pthread_mutex_lock(&mutex);
        if (successfulLoginCount < MAX_CLIENTS) {
            strcpy(userLoginUsername[successfulLoginCount], username);
            successfulLoginCount++;
        }
        pthread_mutex_unlock(&mutex);
        strcpy(response, "LOGIN_SUCCESS");
    } else {
        if (userAlreadyLoggedIn) {
            strcpy(response, "User is already logged in");
        } else {
            strcpy(response, "LOGIN_FAIL");
        }
    }

    // PRINT RESULT
    printf("Result: %s\n", response);

    // Send response to client
    // send(client_socket, response, sizeof(response), 0);
    sendDataToClient(client_socket, response);
}

void handle_register(int client_socket, char* data) {
    // send OK to client
    const char start[256] = "OK";
    char username[256], password[256], confirm_password[256];
    // send(client_socket, start, sizeof(start), 0);
    if (!sendDataToClient(client_socket, start)) return;

    // receive data register from client
    if (!recvDataFromClient(client_socket, username)) return;
    if (!recvDataFromClient(client_socket, password)) return;
    if (!recvDataFromClient(client_socket, confirm_password)) return;

    // Check if password and confirm password match
    if (strcmp(password, confirm_password) != 0) {
        char response[] = "REGISTER_FAIL";
        // send(client_socket, response, sizeof(response), 0);
        sendDataToClient(client_socket, response);
        return;
    }

    char sql[1024];
    sprintf(sql, "INSERT INTO USERS (USERNAME, PASSWORD) VALUES ('%s', '%s');", username, password);
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    char response[256];
    if (rc == SQLITE_OK) {
        strcpy(response, "REGISTER_SUCCESS");
    } else {
        strcpy(response, "REGISTER_FAIL");
    }

    sendDataToClient(client_socket, response);
}

void handle_get_list_player(int client_socket, char* data) {
    const char field_name[10][256] = {"USERNAME", "PASSWORD", "HEALTH", "ATTACK", "LEVEL", "STATUS"};
    const char end[20] = "END";
    char response[256];
    char sql[1024];
    char *err_msg = 0;

    // callback get amount of list player
    int amount_user = 0;
    int callback(void *data, int argc, char **argv, char **azColName) {
        // merge data: format: USERNAME=value;PASSWORD=value;HEALTH=value;ATTACK=value;LEVEL=value;STATUS=value;
        char data_player[256] = "";
        for (int i = 0; i < argc; i++) {
            strcat(data_player, field_name[i]);
            strcat(data_player, "=");
            strcat(data_player, argv[i]);
            strcat(data_player, ";");
        }

        printf("data_player: %s\n", data_player);
        // send data to client
        if (!sendDataToClient(client_socket, data_player)) {
            printf("Error in callback\n");
            return -1;
        };
        amount_user++;
        return 0;
    }

    // get list player
    sprintf(sql, "SELECT USERNAME, PASSWORD FROM USERS;");
    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return;
    }

    // send end of list player to client
    sendDataToClient(client_socket, end);

    printf("Result: %s\n", response);
}

void handle_request_challenge(int client_socket, char* data) {
    const char start[256] = "OK";
    char response[256], username[256], sql[1024];
    int user_available = 0, rc;
    int callback(void *data, int argc, char **argv, char **azColName) {
        user_available = 1;
        return 0;
    }
    char *err_msg = 0;

    // SEND OK TO CLIENT
    sendDataToClient(client_socket, start);

    // RECEIVE USERNAME FROM CLIENT
    recvDataFromClient(client_socket, username);

    printf("Handling request challenge: %s\n", username);

    // CHECK IF STATUS USERNAME IS AVAILABLE IN DATABASE
    sprintf(sql, "SELECT * FROM USERS WHERE USERNAME='%s';", username);
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    printf("Result: %d %d\n", rc, user_available);

    if (rc == SQLITE_OK && user_available == 1) {
        strcpy(response, "ACCEPT");
    } else {
        strcpy(response, "REJECT");
    // }
    // else {
    //     strcpy(response, "TIME_OUT");
    }

    // Send response to client
    sendDataToClient(client_socket, response);
}

void handle_look_request_challenge(int client_socket)
void handle_response_challenge(int client_socket)
void handle_look_response_challenge(int client_socket)
void handle_start_game(int client_socket)


int main(int argc, char* argv[]) {
    if (initialize_database() != 0) {
        return -1;
    }

    fd_set readfds;
    int max_sd, activity, new_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create server socket");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to bind server socket");
        return -1;
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Failed to listen for connections");
        return -1;
    }

    // Khởi tạo tất cả client_sockets thành 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    printf("Server started. Listening on port %d\n", ntohs(server_address.sin_port));

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        if (FD_ISSET(server_socket, &readfds)) {
            new_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
            if (new_socket < 0) {
                perror("accept");
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                if (!recvDataFromClient(sd, buffer)) {
                    close(sd);
                    client_sockets[i] = 0;
                    continue;
                }
                // Xử lý yêu cầu từ client dựa trên buffer
                // handle_login(), handle_register(), ...
                if (strncmp(buffer, "LOGIN", 5) == 0) {
                    handle_login(client_socket, buffer);
                } else if (strncmp(buffer, "REGISTER", 8) == 0) {
                    handle_register(client_socket, buffer);
                } else if (strncmp(buffer, "LIST_PLAYER", 8) == 0) {
                    handle_get_list_player(client_socket, buffer);
                } else if (strncmp(buffer, "REQUEST_CHALLENGE", 8) == 0) {
                    handle_request_challenge(client_socket, buffer);
                } else if (strncmp(buffer, "LOOK_REQUEST_CHALLENGE", 8) == 0) {
                    handle_look_request_challenge(client_socket, buffer);
                } else if (strncmp(buffer, "RESPONSE_CHALLENGE", 8) == 0) {
                    handle_response_challenge(client_socket, buffer);
                } else if (strncmp(buffer, "LOOK_RESPONSE_CHALLENGE", 8) == 0) {
                    handle_look_response_challenge(client_socket, buffer);
                } else if (strncmp(buffer, "START_GAME", 8) == 0) {
                    handle_start_game(client_socket, buffer);
                } else {
                    // Invalid request
                    const char response[256] = "Invalid request";
                    // send(client_socket, response, strlen(response), 0);
                    sendDataToClient(client_socket, response);
                }
            }
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        close(client_sockets[i]);
    }

    close(server_socket);
    sqlite3_close(db);
    return 0;
}
