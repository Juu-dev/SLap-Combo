#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "database.h"
#include "mutex.h"

#define PORT 5003
#define MAX_CONNECTIONS 10

#define CLIENT_IP "127.0.0.1" 

int handle_login(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    char *password = strtok(NULL, ":");
    char *socket_port_invite = strtok(NULL, ":");

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("socket port invite: %s\n", socket_port_invite);

    // Check if username exists
    if (check_username(username) == 0) {
        send(client_socket, "Username does not exist", strlen("Username does not exist"), 0);
        return 1;
    }

    if (check_login_by_username(username) == 1) {
        send(client_socket, "Account has been logged in", strlen("Account has been logged in"), 0);
        return 1;
    }

    // Check if password is correct
    if (check_password(username, password) == 0) {
        send(client_socket, "Password is incorrect", strlen("Password is incorrect"), 0);
        return 1;
    }

    // Update user status to available
    update_status(username, "available");

    // Send response to client
    send(client_socket, "OK", strlen("OK"), 0);

    add_user_account(username, client_socket, atoi(socket_port_invite));

    return 0;
}

int handle_register(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    char *password = strtok(NULL, ":");
    char *confirm_password = strtok(NULL, ":");

    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Confirm password: %s\n", confirm_password);

    // Check if username exists
    if (check_username(username) == 1) {
        send(client_socket, "Username already exists", strlen("Username already exists"), 0);
        return 1;
    }

    // Check if password and confirm_password match
    if (strcmp(password, confirm_password) != 0) {
        send(client_socket, "Password and confirm password do not match", strlen("Password and confirm password do not match"), 0);
        return 1;
    }

    // Create new user
    if (create_new_user(username, password) == 1) {
        send(client_socket, "Error creating new user", strlen("Error creating new user"), 0);
        return 1;
    }
    else {
        send(client_socket, "OK", strlen("OK"), 0);
    }

    // Send response to client

    return 0;
}

int handle_logout(int client_socket, char *buffer) {
    char username[50];
    if (check_login(client_socket) == 0) {
        send(client_socket, "Username does not login", strlen("Username does not login"), 0);
        return 1;
    }

    printf("Complete: %d\n", client_socket);

    // Get username from socket
    get_username(client_socket, username);

    printf("Complete: %s\n", username);

    // Update user status to offline
    update_status(username, "offline");

    // Remove user from userAccounts
    remove_user_account(client_socket);

    // Send response to client
    send(client_socket, "OK", strlen("OK"), 0);

    return 0;
}

int handle_update_state(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    char *state = strtok(NULL, ":");

    printf("Username: %s\n", username);
    printf("State: %s\n", state);

    // Check if username exists
    if (check_username(username) == 0) {
        send(client_socket, "Username does not exist", strlen("Username does not exist"), 0);
        return 1;
    }

    // Update user status
    update_status(username, state);

    // Send response to client
    send(client_socket, "OK", strlen("OK"), 0);

    return 0;
}

int connect_to_client(char *ip, int port) {
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

int handle_get_list_user(int client_socket) {
    char list_user[1024] = "";
    
    get_list_user(list_user);

    printf("List user: %s %ld\n", list_user, strlen(list_user));

    if (strlen(list_user) == 0) {
        strcpy(list_user, "NULL");
    }

    printf("List user: %s %ld\n", list_user, strlen(list_user));

    // send to client
    int byteSend = send(client_socket, list_user, strlen(list_user), 0);
    if (byteSend == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    printf("Send list user success\n");
    return 0;}


int handle_invite(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    int socket_port_invite, socket_port_from;

    printf("----Username----: %s\n", username);

    // Check if username exists
    if (check_username_avail(username) == 0) {
        send(client_socket, "Username does not exist", strlen("Username does not exist"), 0);
        return 1;
    }

    // GET username from client_socket
    char sender_username[50];
    get_username(client_socket, sender_username);

    // get socket port (send invite)
    socket_port_from =  get_socket_port_invite(sender_username);

    // SEND INVITE to client_socket_2
    char message[1024];
    sprintf(message, "INVITE_FROM:%s:%d", sender_username, socket_port_from);

    // get socket_port_invite from username
    socket_port_invite = get_socket_port_invite(username);


    printf("----socket_port_invite----: %d\n", socket_port_invite);

    // connect to client
    int client_socket_2 = connect_to_client(CLIENT_IP, socket_port_invite);

    printf("----connect successly to client_socket_2----: %d\n", client_socket_2);

    // Send message to client

    printf("----Message to client----: %s\n", message);

    send(client_socket_2, message, strlen(message), 0);

    // get socket from username
    // int socket_respondent = get_socket(username);

    // save socket_respondent and socket_petitioner to linked list
    // head = prepend(head, socket_respondent, client_socket);

    // print announce
    // printf("Respondent: %d, Petitioner: %d\n", socket_respondent, client_socket);

    // save to database
    // convert client_socket to string

    // insert_invite(client_socket, socket_respondent);

    // send OK to client
    // send(client_socket, "SEND INVITE OK", strlen("SEND INVITE OK"), 0);

    // Receive response from client
    memset(message, 0, sizeof(message));
    recv(client_socket_2, message, 1024, 0);

    printf("----Message from client----: %s\n", message);

    // Check if invite successful
    if (strcmp(message, "OK") == 0) {
        printf("Invite successful\n");

        char message[1024];
        sprintf(message, "OK:%d", socket_port_invite);

        send(client_socket, message, strlen(message), 0);
    } else {
        printf("Invite failed\n");
        send(client_socket, "Invite failed", strlen("Invite failed"), 0);
    }

    return 0;
}

int handle_add_history(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    char *winner = strtok(NULL, ":");
    char *loser = strtok(NULL, ":");
    char *time = strtok(NULL, ":");

    printf("Username: %s\n", username);
    printf("Winner: %s\n", winner);
    printf("Loser: %s\n", loser);
    printf("Time: %s\n", time);

    // Check if loser exists
    // check if loser is not Machine (not inlcude Machine)
    char* search = "Machine";
    
    if (strstr(winner, search) != NULL) {
        if (check_username(loser) == 0) {
            send(client_socket, "Loser does not exist", strlen("Loser does not exist"), 0);
            return 1;
        }
    } else if (strstr(loser, search) != NULL) {
        if (check_username(winner) == 0) {
            send(client_socket, "Winner does not exist", strlen("Winner does not exist"), 0);
            return 1;
        }
    }
     else {
        // Check if winner exists
        if (check_username(winner) == 0) {
            send(client_socket, "Winner does not exist", strlen("Winner does not exist"), 0);
            return 1;
        }

        if (check_username(loser) == 0) {
            send(client_socket, "Loser does not exist", strlen("Loser does not exist"), 0);
            return 1;
        }

    }

    printf("Check username success\n");
    
    // Insert history
    if (insert_history(username, winner, loser, time) == 1) {
        send(client_socket, "Error inserting history", strlen("Error inserting history"), 0);
        return 1;
    }

    printf("Add history success\n");

    // Send response to client
    send(client_socket, "OK", strlen("OK"), 0);

    return 0;
}

int handle_get_history(int client_socket, char *buffer) {
    char *username = strtok(NULL, ":");
    char history[1024] = "";

    printf("Username: %s\n", username);

    // Check if username exists
    // if (check_username(username) == 0) {
    //     send(client_socket, "Username does not exist", strlen("Username does not exist"), 0);
    //     return 1;
    // }

    printf("Check username success\n");

    // Get history
    if (get_history(username, history) < 0) {
        send(client_socket, "Error getting history", strlen("Error getting history"), 0);
        return 1;
    }

    if (strlen(history) == 0) {
        strcpy(history, "NULL");
    }

    // Send response to client
    send(client_socket, history, strlen(history), 0);

    printf("Get history success: %s\n", history);

    return 0;
}


void *handle_client(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char buffer[1024] = {0};
    int valread;

    while (1) {
        // Read data from the client
        memset(buffer, 0, sizeof(buffer));
        valread = recv(client_socket, buffer, 1024, 0);

        // check if client disconnect
        if (valread == 0) {
            printf("Client disconnected\n");
            break;
        } else if (valread < 0) {
            perror("Error reading from client");
        } else {
            buffer[valread] = '\0';

            printf("=============================================\n");
            printf("Received message from client: %s\n", buffer);
            printf("=============================================\n");

            // Parse the message_type and redirect to the corresponding function
            char *message_type = strtok(buffer, ":");
            printf("Message type: %s\n", message_type);
            if (strcmp(message_type, "LOGIN") == 0) {
                handle_login(client_socket, buffer);
            } else if (strcmp(message_type, "REGISTER") == 0) {
                handle_register(client_socket, buffer);
            } else if (strcmp(message_type, "LOGOUT") == 0) {
                handle_logout(client_socket, buffer);
            } else if (strcmp(message_type, "GET_LIST_USER") == 0) {
                handle_get_list_user(client_socket);
            } else if (strcmp(message_type, "SEND_INVITE") == 0) {
                handle_invite(client_socket, buffer);
            } else if (strcmp(message_type, "UPDATE_STATE") == 0) {
                handle_update_state(client_socket, buffer);
            } else if (strcmp(message_type, "ADD_HISTORY") == 0) {
                handle_add_history(client_socket, buffer);
            } else if (strcmp(message_type, "GET_HISTORY") == 0) {
                handle_get_history(client_socket, buffer);
            // } else if (strcmp(message_type, "PETITIONER_CHECK") == 0) {
            //     handle_petitioner_check(client_socket, buffer);
            } else {
                printf("Invalid message type\n");
            }

            // Echo the message back to the client
            // send(client_socket, buffer, strlen(buffer), 0);
        }
    }

    // Close the socket and free the thread's socket descriptor
    close(client_socket);
    free(socket_desc);

    return NULL;
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Initialize the database
    initialize_database();
    create_users_table();
    create_invitation_table();
    create_history_table();

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    //     perror("Setsockopt failed");
    //     exit(EXIT_FAILURE);
    // }

    // Initialize server address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a new connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("New connection accepted\n");

        // Create a new thread to handle the client
        pthread_t thread;
        int *new_socket = malloc(1);
        *new_socket = client_socket;

        if (pthread_create(&thread, NULL, handle_client, (void *)new_socket) < 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Detach the thread to avoid memory leaks
        pthread_detach(thread);
    }

    return 0;
}
