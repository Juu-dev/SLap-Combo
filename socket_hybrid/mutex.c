#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_CONNECTIONS 10

typedef struct {
    char username[50];
    int socket;
    int socket_port_invite;
} UserAccount;


UserAccount userAccounts[MAX_CONNECTIONS];
int numUserAccounts = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void add_user_account(const char *username, int socket, int socket_port_invite) {
    pthread_mutex_lock(&mutex);

    // Add user to userAccounts
    strcpy(userAccounts[numUserAccounts].username, username);
    userAccounts[numUserAccounts].socket = socket;  
    userAccounts[numUserAccounts].socket_port_invite = socket_port_invite;
    numUserAccounts++;

    pthread_mutex_unlock(&mutex);
}

void remove_user_account(int socket) {
    pthread_mutex_lock(&mutex);

    // Remove user from userAccounts
    for (int i = 0; i < numUserAccounts; i++) {
        if (userAccounts[i].socket == socket) {
            for (int j = i; j < numUserAccounts - 1; j++) {
                strcpy(userAccounts[j].username, userAccounts[j + 1].username);
                userAccounts[j].socket = userAccounts[j + 1].socket;
            }
            numUserAccounts--;
            break;
        }
    }

    pthread_mutex_unlock(&mutex);
}

// check if username login or not with socket
int check_login(int socket) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < numUserAccounts; i++) {
        if (userAccounts[i].socket == socket) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
    }

    pthread_mutex_unlock(&mutex);
    return 0;
}

// get username from socket
void get_username(int socket, char *username) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < numUserAccounts; i++) {
        if (userAccounts[i].socket == socket) {
            pthread_mutex_unlock(&mutex);
            strcpy(username, userAccounts[i].username);
        }
    }

    pthread_mutex_unlock(&mutex);
    return;
}

int get_socket_port_invite(char *username) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < numUserAccounts; i++) {
        if (strcmp(userAccounts[i].username, username) == 0) {
            pthread_mutex_unlock(&mutex);
            return userAccounts[i].socket_port_invite;
        }
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}