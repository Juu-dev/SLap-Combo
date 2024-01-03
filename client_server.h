#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

int handle_login(char* username, char* password, int server_socket, char* socket_port_invite);
int handle_register(int server_socket, char* username, char* password, char* confirm_password);
int handle_logout(int server_socket);
char* handle_send_invite(int server_socket, char* username);
void start_game(int socket_port);

int handle_respondent_check(int server_socket);
int handle_get_list_user(int server_socket, char* list_user);
int handle_respondent_response(int server_socket, char* message);
int handle_petitioner_check(int server_socket, char* username);

#endif