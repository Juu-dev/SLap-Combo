#ifndef ACTION_CLIENT_UTILS_H
#define ACTION_CLIENT_UTILS_H

void action_my_turn(int server_socket, int index_struct, int my_socket) {
    char attack[50];
    char buffer[1024];

    printf("Attack: ");
    scanf("%49s", attack); // Sử dụng %49s để giới hạn kích thước đầu vào

    // send to server
    sprintf(buffer, "attack:%d:%d:%s", index_struct, my_socket, attack);
    send(server_socket, buffer, strlen(buffer), 0);
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
                action_my_turn(server_socket, index_struct, my_socket);
            }
        }
    }
}

#endif