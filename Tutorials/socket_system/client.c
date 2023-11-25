#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void sendChallenge(int sock, int idA, int idB) {
    char message[50];
    sprintf(message, "challenge:%d:%d", idA, idB);
    send(sock, message, strlen(message), 0);
    printf("Challenge sent to player %d\n", idB);
}

// Xử lý tin nhắn đến từ server
void handleIncomingMessage(int sock) {
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';

    // Xử lý tin nhắn ở đây
    // Ví dụ: "You have been challenged by player X"
    int challengerID;
    if (sscanf(buffer, "You have been challenged by player %d", &challengerID) == 1) {
        printf("%s\n", buffer);
        // Gửi phản hồi cho server (chấp nhận hoặc từ chối)
        // ...
    }
}

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Challenge request"; // Tin nhắn gửi tới server
    char buffer[1024] = {0};

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IPv4 và IPv6 từ text sang binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Kết nối tới server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Gửi yêu cầu thách đấu
    send(sock, hello, strlen(hello), 0);
    printf("Challenge request sent\n");

    // Gửi yêu cầu thách đấu (ví dụ: player 1 thách đấu player 2)
    sendChallenge(sock, 1, 2);

    // Đọc phản hồi từ server
     while (true) {
        handleIncomingMessage(sock);
        // Thêm logic để xử lý các sự kiện khác, như ngắt kết nối
    }

    close(sock);

    return 0;
}
