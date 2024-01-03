#ifndef DATABASE_H
#define DATABASE_H

int initialize_database();
int create_users_table();
int create_new_user(char *username, char *password);
int update_status(char *username, char *status);
int check_username(char *username);
int check_username_avail(char *username);
int check_password(char *username, char *password);
int get_list_user(char *list_user);

int create_history_table();
int insert_history(char *username, char *winner, char *loser, char *time);
int get_history(char *username, char *history);

int create_invitation_table();
int insert_invite(int petitioner, int respondent);
int update_petitioner_status(int respondent, int petitioner, char *status);
int update_respondent_status(int respondent, int petitioner, char *status);
int get_petitioner_invite_by_respondent(int respondent, char *petitioner_invite);
int get_respondent_status(int respondent, int petitioner, char *respondent_status);
int delete_invite_by_respondent(int respondent);

#endif