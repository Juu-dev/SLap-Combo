#ifndef ACTION_CLIENT_H
#define ACTION_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "action_client_utils.h"

int action_login(int server_socket) {
    char buffer[1024];

    char username[50];
    char password[50];

    printf("Username: ");
    scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào
    printf("Password: ");
    scanf("%49s", password);

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

int action_register(int server_socket) {
    char buffer[1024];
    // Nhập thông tin đăng ký
    char username[50];
    char password[50];
    char confirm_password[50];

    printf("Username: ");
    scanf("%49s", username); // Sử dụng %49s để giới hạn kích thước đầu vào
    printf("Password: ");
    scanf("%49s", password);
    printf("Confirm password: ");
    scanf("%49s", confirm_password);

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "register:%s:%s:%s", username, password, password);
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

void action_get_list_users(int server_socket) {
    char buffer[256];

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "get_list_users");
    send(server_socket, buffer, strlen(buffer), 0);

    // // Nhận phản hồi từ server
    // memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    // recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);

    printf("*****************************************\n");
    printf("Server response: %s\n", buffer);
    printf("*****************************************\n");

    printf("[COMPLETE GET LIST USERS FUNCTION]\n");
}

void wait_invite(int server_socket) {
    char buffer[1024];
    char username[256];

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer)); // Xóa buffer
    recv(server_socket, buffer, sizeof(buffer) - 1, 0);
    // split buffer to get username
    char *token = strtok(buffer, ":");
    token = strtok(NULL, ":");
    printf("*****************************************\n");
    printf("Server response: Invite from %s\n", token);
    printf("*****************************************\n");

    action_response_invite(server_socket, token);

    printf("[COMPLETE WAIT INVITE FUNCTION]\n");
}

int action_invite(int server_socket, char *username) {
    char buffer[1024];
    // char username[50];
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
    char *token = strtok(buffer, ":");
    token = strtok(NULL, ":");
    strcpy(username, token);
    token = strtok(NULL, ":");
    strcpy(response, token);

    // send start game to server
    if (strcmp(response, "accept") == 0) {
        memset(buffer, 0, sizeof(buffer)); // Xóa buffer
        sprintf(buffer, "start_game:%s", username);
        send(server_socket, buffer, strlen(buffer), 0);
        action_wait_start_game(server_socket);
        return 0;
    }
    else {
        return -1;
    }

    printf("[COMPLETE INVITE FUNCTION]\n");
}

int action_response_invite(int server_socket, char *username) {
    char buffer[1024];
    char response[50];

    printf("Response: ");
    scanf("%49s", response); // Sử dụng %49s để giới hạn kích thước đầu vào

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "response_invite:%s:%s", username, response);
    send(server_socket, buffer, strlen(buffer), 0);

    if (strcmp(response, "accept") == 0) {
        printf("***************OK***************\n");
        action_wait_start_game(server_socket);
        return 0;
    }
    else {
        return -1;
    }

    printf("[COMPLETE RESPONSE INVITE FUNCTION]\n");
}

void action_wait_start_game(int server_socket) {
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
        start_game(server_socket);
        printf("*****************************************\n");
    }
}

void action_update_status(int server_socket) {
    char buffer[256];
    char status[50];

    printf("Status: ");
    scanf("%49s", status);

    // Gửi thông tin đăng ký tới server
    sprintf(buffer, "update_status: %s", status);
    send(server_socket, buffer, strlen(buffer), 0);

    printf("[COMPLETE UPDATE STATUS FUNCTION]\n");
}

#endif