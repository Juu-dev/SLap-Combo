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

void handle_login(int client_socket, char* data) {
    // send OK to client
    char start[256] = "OK";
    send(client_socket, start, sizeof(start), 0);
    // TODO: Implement login logic
    // Example:
    printf("Handling login.\n");
    // receive data login from client
    char username[256];
    char password[256];

    recv(client_socket, username, sizeof(username), 0);
    recv(client_socket, password, sizeof(password), 0);

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);

    // Send response to client
    char response[] = "LOGIN_SUCCESS";
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

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Confirm password: %s\n", confirm_password);

    // Send response to client
    char response[] = "REGISTER_SUCCESS";
    send(client_socket, response, sizeof(response), 0);
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
    int port = argc > 1 ? atoi(argv[1]) : 8080;
    start_server(port);
    return 0;
}
