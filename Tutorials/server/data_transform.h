// code format for file .h
#ifndef DATA_TRANSFORM_H
#define DATA_TRANSFORM_H

bool sendDataToClient(int client_socket, char* data) {
    int sendBytes = send(client_socket, data, 256, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi phản hồi cho máy khach");
        return false;
    }
    return true;
}

bool sendTagFunction(int client_socket, const char* data) {
    int sendBytes = send(client_socket, data, 20, 0);
    if (sendBytes < 0) {
        perror("Lỗi gửi phản hồi cho máy khach");
        return false;
    }
    return true;
}

bool recvDataFromClient(int client_socket, char* data) {
    int recvBytes = recv(client_socket, data, 256, 0);
    if (recvBytes < 0) {
        perror("Lỗi nhận phản hồi từ máy khach");
        return false;
    }
    data[recvBytes] = '\0';
    return true;
}

#endif