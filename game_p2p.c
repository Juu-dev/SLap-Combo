#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

void user_send(int socket) {
    char buffer[1024] = "";

    printf("Enter attack: ");
    scanf("%s", buffer);

    int bytes_sent = send(socket, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

}

void user_recv(int socket) {
    char buffer[1024] = "";
     int bytes_received = recv(socket, buffer, 1024, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }
    
    buffer[bytes_received] = '\0';

    printf("Received Game: %s\n", buffer);
}
