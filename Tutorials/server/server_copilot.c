#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "data_transform.h"
#include "database.h"

#define MAX_CLIENTS 10

typedef struct {
    int round;
    int health_1;
    int health_2;
    int socket_in_round;
} GameState;
GameState gameState;

int server_socket;
int client_sockets[MAX_CLIENTS];

pthread_t client_threads[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char userLoginUsername[MAX_CLIENTS][256];
char socket_request_challenge[256] = "";
char socket_response_challenge[256] = "";
char socket_current_round[256] = "";
char response_challenge[256] = "";
bool isGameStarted = false;
bool isGameFinished = false;
int successfulLoginCount = 0;

void handle_playing_game(int client_socket) {
    printf("Handling playing game\n");
    bool isLoop = true;
    while (isLoop) {
        printf("Waiting for playing game %d\n", client_socket);
        pthread_mutex_lock(&mutex);
        if (!isGameFinished) {
            printf("Playing game %d, %d\n", client_socket, gameState.socket_in_round);
            // if socket_current_round == client_socket => receive data from client
            if (gameState.socket_in_round == client_socket) {
                // receive data from client
                char data[256];
                recvDataFromClient(client_socket, data);
                // update game state
                printf("Attack from %d: %s\n", client_socket, data);
                if (gameState.socket_in_round == atoi(socket_request_challenge)) {
                    gameState.socket_in_round = atoi(socket_response_challenge);
                }
                else {
                    gameState.socket_in_round = atoi(socket_request_challenge);
                }
                printf("socket_in_round: %d\n", gameState.socket_in_round);
            }
            else {
                const char field_name[10][256] = {"ROUND", "HEALTH_1", "HEALTH_2", "SOCKET_IN_ROUND"};
                char * delimiter = ";";

                // merge to string
                char data[256] = "";
                char temp[256];
                for (int i = 0; i < 4; i++) {
                    strcat(data, field_name[i]);
                    strcat(data, "=");
                    if (i == 0) {
                        sprintf(temp, "%d", gameState.round);
                        strcat(data, temp);
                    }
                    else if (i == 1) {
                        sprintf(temp, "%d", gameState.health_1);
                        strcat(data, temp);
                    }
                    else if (i == 2) {
                        sprintf(temp, "%d", gameState.health_2);
                        strcat(data, temp);
                    }
                    else if (i == 3) {
                        sprintf(temp, "%d", gameState.socket_in_round);
                        strcat(data, temp);
                    }
                    strcat(data, delimiter);
                }

                // send data to client
                sendDataToClient(client_socket, data);
            }
        }
        else {
            isLoop = false;
        }
        pthread_mutex_unlock(&mutex);
    }

}

void start_game_two_player(int client_socket) {
    bool isLoop = true;
    while (isLoop) {
        pthread_mutex_lock(&mutex);
        printf("Waiting for start game %d\n", client_socket);
        if (isGameStarted) {
            sendDataToClient(client_socket, "START_GAME");
            isLoop = false;
        }
        pthread_mutex_unlock(&mutex);
    }
    
    handle_playing_game(client_socket);
}

void handle_login(int client_socket, char* data) {
    // send OK to client
    char start[256] = "OK", username[256], password[256], sql[1024];
    int user_exists = 0, rc;
    int callback(void *data, int argc, char **argv, char **azColName) {
        user_exists = 1;
        return 0;
    }

    if (!sendDataToClient(client_socket, start)) return;
    printf("Handling login.\n");

    // receive data login from client
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
        // update SOCKET IN DATABASE
        sprintf(sql, "UPDATE USERS SET SOCKET='%d' WHERE USERNAME='%s';", client_socket, username);
        char *err_msg = 0;
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

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

    // send(client_socket, response, sizeof(response), 0);
    sendDataToClient(client_socket, response);
}

void handle_register(int client_socket, char* data) {
    // send OK to client
    const char start[256] = "OK";
    char username[256], password[256], confirm_password[256];
    // send(client_socket, start, sizeof(start), 0);
    if (!sendTagFunction(client_socket, start)) return;

    // receive data register from client
    if (!recvDataFromClient(client_socket, username)) return;
    if (!recvDataFromClient(client_socket, password)) return;
    if (!recvDataFromClient(client_socket, confirm_password)) return;

    printf("username: %s\n", username);
    printf("password: %s\n", password);
    printf("confirm_password: %s\n", confirm_password);

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
    char response[256], sql[1024];
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
    sendTagFunction(client_socket, end);
}

void handle_request_challenge(int client_socket, char* data) {
    const char start[256] = "OK";
    char response[256], username[256], sql[1024];
    int user_socket = 0, rc;
    int callback(void *data, int argc, char **argv, char **azColName) {
        // user_socket = 1;
        user_socket = atoi(argv[0]);
        return 0;
    }
    char *err_msg = 0;

    // SEND OK TO CLIENT
    sendTagFunction(client_socket, start);

    // RECEIVE USERNAME FROM CLIENT
    recvDataFromClient(client_socket, username);

    // CHECK IF STATUS USERNAME IS AVAILABLE IN DATABASE
    sprintf(sql, "SELECT SOCKET FROM USERS WHERE USERNAME='%s';", username);
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    // update socket_request_challenge
    pthread_mutex_lock(&mutex);
    if (user_socket != 0) {
        sprintf(socket_request_challenge, "%d", user_socket);
        sprintf(socket_response_challenge, "%d", client_socket);
    }
    pthread_mutex_unlock(&mutex);

    char response_content[256];
    while (1) {
        pthread_mutex_lock(&mutex);
        if (response_challenge[0] != '\0') {
            strcpy(response_content, response_challenge);
            response_challenge[0] = '\0';
        }
        pthread_mutex_unlock(&mutex);

        // check if response_challenge is ACCEPT or REJECT
        if (strcmp(response_content, "ACCEPT") == 0 || strcmp(response_content, "REJECT") == 0) {
            sendDataToClient(client_socket, response_content);
            break;
        }
    }
}

void handle_look_request_challenge(int client_socket) {
    char socket_request[256];
    while (1) {
        pthread_mutex_lock(&mutex);
        if (socket_request_challenge[0] != '\0') {
            strcpy(socket_request, socket_request_challenge);
            // socket_request_challenge[0] = '\0';
        }
        pthread_mutex_unlock(&mutex);

        // check if socket_request is not empty
        if (socket_request[0] != '\0') {
            sendDataToClient(client_socket, socket_request);
            break;
        }
    }
}

void handle_response_challenge(int client_socket) {
    char response[256];
    // receive response from client
    recvDataFromClient(client_socket, response);

    // send response to socket_request
    pthread_mutex_lock(&mutex);
    strcpy(response_challenge, response);
    pthread_mutex_unlock(&mutex);

    start_game_two_player(client_socket);
}

void handle_start_game(int client_socket) {
    printf("Handling start game\n");

    pthread_mutex_lock(&mutex);
    isGameStarted = true;
    gameState.round = 1;
    gameState.health_1 = 100;
    gameState.health_2 = 100;
    gameState.socket_in_round = atoi(socket_request_challenge);
    pthread_mutex_unlock(&mutex);

    printf("Start game\n");

    start_game_two_player(client_socket);
}

void* client_handler(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;

    char buffer[256];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        // int bytes_received = recv(client_socket, buffer, 256, 0);
        // if (bytes_received <= 0) {
        //     // Client disconnected
        //     printf("Client disconnected\n");
        //     break;
        // }
        if (!recvDataFromClient(client_socket, buffer)) break;
        // Process received data
        printf("Received data from client: %s\n", buffer);
        
        // Check the type of request
        if (strncmp(buffer, "LOGIN", 5) == 0) {
            handle_login(client_socket, buffer);
        } else if (strncmp(buffer, "REGISTER", 8) == 0) {
            handle_register(client_socket, buffer);
        } else if (strncmp(buffer, "LIST_PLAYER", 8) == 0) {
            handle_get_list_player(client_socket, buffer);
        } else if (strncmp(buffer, "REQUEST_CHALLENGE", 8) == 0) {
            handle_request_challenge(client_socket, buffer);
        } else if (strncmp(buffer, "LOOK_REQUEST_CHALLENGE", 8) == 0) {
            handle_look_request_challenge(client_socket);
        } else if (strncmp(buffer, "RESPONSE_CHALLENGE", 8) == 0) {
            handle_response_challenge(client_socket);
        } else if (strncmp(buffer, "START_GAME", 8) == 0) {
            handle_start_game(client_socket);
        } else if (strncmp(buffer, "ATTACK", 8) == 0) {
            handle_playing_game(client_socket);
        } else {
            // Invalid request
            const char response[256] = "Invalid request";
            // send(client_socket, response, strlen(response), 0);
            sendTagFunction(client_socket, response);
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
