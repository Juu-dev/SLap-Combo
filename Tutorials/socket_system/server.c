#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 30

struct ChallengeRequest {
    int challengerID;
    int challengeeID;
    int challengeeSocket;
} challengeRequests[MAX_CLIENTS];

// int challengeRequests[MAX_CLIENTS][2]; // Lưu trữ yêu cầu thách đấu [A, B]
memset(challengeRequests, -1, sizeof(challengeRequests)); // Khởi tạo với -1

// Hàm gửi yêu cầu thách đấu
void sendChallengeRequest(int challengerID, int challengeeID) {
    int challengeeSocket = playerSockets[challengeeID];
    if (challengeeSocket != 0) {
        char message[50];
        sprintf(message, "You have been challenged by player %d", challengerID);
        send(challengeeSocket, message, strlen(message), 0);
    }
}

void forwardChallengeResponse(int challengeeID, const char* response) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (challengeRequests[i].challengeeID == challengeeID) {
            int challengerSocket = playerSockets[challengeRequests[i].challengerID];
            if (challengerSocket != 0) {
                send(challengerSocket, response, strlen(response), 0);
            }
            break;
        }
    }
}

int main() {
    int opt = 1;
    int master_socket, addrlen, new_socket, client_socket[MAX_CLIENTS];
    int max_sd, sd, activity;
    struct sockaddr_in address;
    fd_set readfds;

    // Khởi tạo tất cả client_socket[] thành 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // Tạo master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập master socket để cho phép nhiều kết nối
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ cho socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Ràng buộc socket tới địa chỉ cụ thể
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Bắt đầu lắng nghe kết nối đến
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Chấp nhận kết nối đến
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1) {
        // Làm sạch tập hợp socket
        FD_ZERO(&readfds);

        // Thêm master socket vào tập hợp
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Thêm child sockets vào tập hợp
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // Lấy file descriptor
            sd = client_socket[i];

            // Nếu file descriptor hợp lệ, thêm vào tập hợp đọc
            if (sd > 0) FD_SET(sd, &readfds);

            // Lưu file descriptor lớn nhất
            if (sd > max_sd) max_sd = sd;
        }

        // Chờ sự kiện trên một trong các socket (sử dụng timeout nếu cần)
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // Nếu có sự kiện trên master socket, đó là một kết nối đến mới
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Thêm socket mới vào mảng socket
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Kiểm tra sự kiện trên các socket khác
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Đọc dữ liệu từ client
                char buffer[1024];
                int valread = read(sd, buffer, 1024);
                buffer[valread] = '\0';

                // Giả định dữ liệu là "challenge:ID_A:ID_B"
                int idA, idB;
                if (sscanf(buffer, "challenge:%d:%d", &idA, &idB) == 2) {
                    // Lưu trữ yêu cầu thách đấu
                    challengeRequests[idA][0] = idA;
                    challengeRequests[idA][1] = idB;

                    // Gửi yêu cầu thách đấu tới B...
                    sendChallengeRequest(idA, idB);
                }

                int idB;
                char response[50];
                if (sscanf(buffer, "response:%d:%s", &idB, response) == 2) {
                    forwardChallengeResponse(idB, response);
                }
            }
        }
    }

    return 0;
}
