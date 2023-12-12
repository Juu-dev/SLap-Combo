#ifndef DATABASE_H
#define DATABASE_H

int initialize_database();
int create_users_table();
int create_new_user(char *username, char *password);
int update_status(char *username, char *status);
int check_username(char *username);
int check_password(char *username, char *password);

#endif