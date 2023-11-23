#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sqlite3.h>

#define MAX_CLIENTS 10

sqlite3 *db;

int server_socket;
int client_sockets[MAX_CLIENTS];
pthread_t client_threads[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char userLoginUsername[MAX_CLIENTS][256];
int successfulLoginCount = 0;

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

int create_table_game_play() {
    // Create table
    char *sql = "CREATE TABLE IF NOT EXISTS GAME_PLAYS("  \
                "ROUND TEXT NOT NULL," \
                "GAME_ID TEXT PRIMARY KEY NOT NULL," \
                "PLAYER_1 TEXT NOT NULL," \
                "PLAYER_2 TEXT NOT NULL," \
                "PLAYER_1_NAME TEXT NOT NULL," \
                "WINNER TEXT NULL);";

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    } else {
        fprintf(stdout, "GAME_PLAYS Table created successfully\n");
    }
    return 0;
}

bool sendDataToClient(int client_socket, char* data) {
    int sendBytes = send(client_socket, data, 256, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi phản hồi cho máy chủ");
        return false;
    }
    return true;
}

bool sendDataToClientConst(int client_socket, const char* data) {
    int sendBytes = send(client_socket, data, 20, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi phản hồi cho máy chủ");
        return false;
    }
    return true;
}

bool recvDataFromClient(int client_socket, char* data) {
    int recvBytes = recv(client_socket, data, sizeof(data), 0);
    if (recvBytes < 0) {
        perror("Lỗi nhận phản hồi từ máy khach");
        return false;
    }
    data[recvBytes] = '\0';
    return true;
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

    if (!recv(client_socket, username, sizeof(username), 0)) return;
    if (!recv(client_socket, password, sizeof(password), 0)) return;

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
    send(client_socket, response, sizeof(response), 0);
}

void handle_register(int client_socket, char* data) {
    // send OK to client
    char start[256] = "OK";
    send(client_socket, start, sizeof(start), 0);
    // TODO: Implement register logic
    // Example:
    printf("Handling register: %s\n", data);

    char username[256];
    char password[256];
    char confirm_password[256];

    recv(client_socket, username, sizeof(username), 0);
    recv(client_socket, password, sizeof(password), 0);
    recv(client_socket, confirm_password, sizeof(confirm_password), 0);

    // printf("Username: %s\n", username);
    // printf("Password: %s\n", password);
    // printf("Confirm password: %s\n", confirm_password);

    // Check if password and confirm password match
    if (strcmp(password, confirm_password) != 0) {
        char response[] = "REGISTER_FAIL";
        send(client_socket, response, sizeof(response), 0);
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

    printf("Result: %s\n", response);
    // Send response to client
    send(client_socket, response, sizeof(response), 0);
}

void handle_get_list_player(int client_socket, char* data) {
    const char field_name[10][256] = {"USERNAME", "PASSWORD", "HEALTH", "ATTACK", "LEVEL", "STATUS"};
    const char end[20] = "END";

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
        sendDataToClient(client_socket, data_player);
        amount_user++;
        return 0;
    }

    // get list player
    char sql[1024];
    sprintf(sql, "SELECT USERNAME, PASSWORD FROM USERS;");
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return;
    }

    // send amount of list player to client
    char response[256];
    // send end of list player to client
    sendDataToClientConst(client_socket, end);

    printf("Result: %s\n", response);
}

void* client_handler(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;

    char buffer[256];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            // Client disconnected
            printf("Client disconnected\n");
            break;
        }
        // Process received data
        printf("Received data from client: %s\n", buffer);
        
        // Check the type of request
        if (strncmp(buffer, "LOGIN", 5) == 0) {
            handle_login(client_socket, buffer);
        } else if (strncmp(buffer, "REGISTER", 8) == 0) {
            handle_register(client_socket, buffer);
        } else if (strncmp(buffer, "LIST_PLAYER", 8) == 0) {
            handle_get_list_player(client_socket, buffer);
        } else if (strncmp(buffer, "CHALLENGE", 8) == 0) {
            // handle_get_list_player(client_socket, buffer);
        } else {
            // Invalid request
            char response[] = "Invalid request";
            send(client_socket, response, strlen(response), 0);
        }
    }

    close(client_socket);
    pthread_exit(NULL);
}

int start_server(int port) {
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create server socket");
        return -1;
    }

    // Bind server socket to port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to bind server socket");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Failed to listen for connections");
        return -1;
    }

    printf("Server started. Listening on port %d\n", port);

    // Accept and handle client connections
    int client_count = 0;
    while (1) {
        printf("Waiting for incoming connections...\n");
        // Accept client connection
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Failed to accept client connection");
            continue;
        }

        // Create a new thread for each client
        pthread_create(&client_threads[client_count], NULL, client_handler, &client_socket);

        // Store the client socket in the array
        client_sockets[client_count] = client_socket;

        // Increment client count
        client_count++;

        // Check if maximum number of clients reached
        if (client_count >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. Cannot accept more connections.\n");
            break;
        }
    }

    // Wait for all client threads to finish
    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    // Close all client sockets
    for (int i = 0; i < client_count; i++) {
        close(client_sockets[i]);
    }

    // Close server socket
    close(server_socket);
    return 0;
}

int main(int argc, char* argv[]) {
    if (initialize_database() != 0) {
        return -1;
    }

    int port = argc > 1 ? atoi(argv[1]) : 8080;
    start_server(port);
    
    sqlite3_close(db);
    return 0;
}
