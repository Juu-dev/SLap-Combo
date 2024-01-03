#ifndef MUTEX_H
#define MUTEX_H

void add_user_account(const char *username, int socket, int socket_port_invite);
void remove_user_account(int socket);
int check_login_by_username(char *username);
int check_login(int socket);
void get_username(int socket, char *username);
int get_socket_port_invite(char *username);
int get_socket(char *username);

#endif