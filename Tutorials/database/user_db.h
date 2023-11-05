#ifndef USERDB_H
#define USERDB_H

int connect_to_db();
int register_user(const char *username, const char *password);
int login_user(const char *username, const char *password);
int logout_user();

#endif
