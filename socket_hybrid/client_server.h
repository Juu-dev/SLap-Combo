#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

int handle_login(int server_socket, int *port_socket_invite);
int handle_register(int server_socket);
int handle_logout(int server_socket);
int handle_send_invite(int server_socket);

#endif