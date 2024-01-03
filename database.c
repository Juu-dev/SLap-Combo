#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *db;

struct user_account {
    int id;
    char username[50];
    char password[50];
    char status[50];
};

struct user_account users[10];

int initialize_database();
int create_users_table();
int create_new_user(char *username, char *password);
int update_status(char *username, char *status);
int check_username(char *username);
int check_password(char *username, char *password);
int get_list_user(char *list_user);

int initialize_database() {
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Opened database successfully\n");
    }
    return 0;
}

// table users: id, username, password, status,
// status: available, playing, offline (default is offline)
// username: unique
int create_users_table() {
    char *sql = "CREATE TABLE IF NOT EXISTS users(" \
        "id INTEGER PRIMARY KEY AUTOINCREMENT," \
        "username TEXT UNIQUE," \
        "password TEXT NOT NULL," \
        "status TEXT NOT NULL DEFAULT 'offline',"\
        "ip TEXT NOT NULL DEFAULT '');";
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    else {
        fprintf(stdout, "Table users created successfully\n");
        create_new_user("admin_1", "1");
        create_new_user("admin_2", "1");
        create_new_user("admin_3", "1");
    }
    return 0;
}

int create_new_user(char *username, char *password) {
    char *sql = "INSERT INTO users (username, password) VALUES (?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    // sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    // sqlite3_bind_text(stmt, 2, password, strlen(password), 0);
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, password, strlen(password), 0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Insert user successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;

}

int update_status(char *username, char *status) {
    char *sql = "UPDATE users SET status = ? WHERE username like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, status, strlen(status), 0);
    sqlite3_bind_text(stmt, 2, username, strlen(username), 0);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Update status successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int check_username(char *username) {
    char *sql = "SELECT * FROM users WHERE username like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        return 1;
    }
    else {
        return 0;
    }
}

int check_username_avail(char *username) {
    char *sql = "SELECT * FROM users WHERE username like ? and status like 'available'";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        return 1;
    }
    else {
        return 0;
    }
}

int check_password(char *username, char *password) {
    char *sql = "SELECT * FROM users WHERE username like ? AND password like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, password, strlen(password), 0);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        return 1;
    }
    else {
        return 0;
    }
}

int get_list_user(char *list_user) {
    char *sql = "SELECT * FROM users WHERE status like 'available'";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int count = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    while (1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char *username = (char *)sqlite3_column_text(stmt, 1);
            char *password = (char *)sqlite3_column_text(stmt, 2);
            char *status = (char *)sqlite3_column_text(stmt, 3);

            users[count].id = id;
            strcpy(users[count].username, username);
            strcpy(users[count].password, password);
            strcpy(users[count].status, status);
            
            count++;
        }
        else {
            break;
        }
    }

    // merge all information into a tring with format: id:username:password:status;id:username:password:status;...
    strcpy(list_user, "");
    for (int i = 0; i < count; i++) {
        char *user = (char *)malloc(1024);
        sprintf(user, "%d:%s:%s:%s;", users[i].id, users[i].username, users[i].password, users[i].status);
        strcat(list_user, user);
    }

    sqlite3_finalize(stmt);
    return 0;
}

int update_ip_user(char *username, char *ip) {
    char *sql = "UPDATE users SET ip = ? WHERE username like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    printf("username in sql: %s\n", username);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, ip, strlen(ip), 0);
    sqlite3_bind_text(stmt, 2, username, strlen(username), 0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;

}

int get_ip_user(char *username, char *ip) {
    char *sql = "SELECT * FROM users WHERE username like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int count = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);

    printf("username in sql: %s\n", username);

    while (1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char *username = (char *)sqlite3_column_text(stmt, 1);
            char *password = (char *)sqlite3_column_text(stmt, 2);
            char *status = (char *)sqlite3_column_text(stmt, 3);
            char *ip_exp = (char *)sqlite3_column_text(stmt, 4);

            strcpy(ip, ip_exp);
            count++;
        }
        else {
            break;
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}


// table invitations: id, petitioner, respondent, petitioner_status, respondent_status
int create_invitation_table() {
    char *sql = "CREATE TABLE IF NOT EXISTS invitations(" \
        "id INTEGER PRIMARY KEY AUTOINCREMENT," \
        "petitioner TEXT NOT NULL," \
        "respondent TEXT NOT NULL," \
        "petitioner_status TEXT NOT NULL DEFAULT 'pending'," \
        "respondent_status TEXT NOT NULL DEFAULT 'pending');";
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    else {
        fprintf(stdout, "Table invitations created successfully\n");
    }
    return 0;
}

int insert_invite(int petitioner, int respondent) {
    char *sql = "INSERT INTO invitations (petitioner, respondent) VALUES (?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, petitioner);
    sqlite3_bind_int(stmt, 2, respondent);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Insert invite successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int update_petitioner_status(int respondent, int petitioner, char *status) {
    char *sql = "UPDATE invitations SET petitioner_status = ? WHERE respondent like ? AND petitioner like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_text(stmt, 1, status, strlen(status), 0);
    sqlite3_bind_int(stmt, 2, respondent);
    sqlite3_bind_int(stmt, 3, petitioner);


    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Update petitioner status successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int update_respondent_status(int respondent, int petitioner, char *status) {
    char *sql = "UPDATE invitations SET respondent_status = ? WHERE respondent like ? AND petitioner like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, status, strlen(status), 0);
    sqlite3_bind_int(stmt, 2, respondent);
    sqlite3_bind_int(stmt, 3, petitioner);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Update respondent status successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int get_petitioner_invite_by_respondent(int respondent, char *petitioner_invite) {
    char *sql = "SELECT * FROM invitations WHERE respondent like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int count = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_int(stmt, 1, respondent);

    while (1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char *petitioner = (char *)sqlite3_column_text(stmt, 1);
            char *respondent = (char *)sqlite3_column_text(stmt, 2);
            char *petitioner_status = (char *)sqlite3_column_text(stmt, 3);
            char *respondent_status = (char *)sqlite3_column_text(stmt, 4);
            
            strcmp(petitioner_invite, petitioner);

            count++;
        }
        else {
            break;
        }
    }

    // merge all information into a tring with format: id:username:password:status;id:username:password:status;...
    sqlite3_finalize(stmt);
    return 0;
}

int get_respondent_status(int respondent, int petitioner, char *respondent_status) {
    char *sql = "SELECT * FROM invitations WHERE respondent like ? AND petitioner like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int count = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_int(stmt, 1, respondent);
    sqlite3_bind_int(stmt, 2, petitioner);

    while (1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char *petitioner = (char *)sqlite3_column_text(stmt, 1);
            char *respondent = (char *)sqlite3_column_text(stmt, 2);
            char *petitioner_status = (char *)sqlite3_column_text(stmt, 3);
            char *respondent_status_exp = (char *)sqlite3_column_text(stmt, 4);
            
            strcmp(respondent_status, respondent_status_exp);

            count++;
        }
        else {
            break;
        }
    }

    // merge all information into a tring with format: id:username:password:status;id:username:password:status;...
    sqlite3_finalize(stmt);
    return 0;
}

int delete_invite_by_respondent(int respondent) {
    char *sql = "DELETE FROM invitations WHERE respondent like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_int(stmt, 1, respondent);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// ======================== HISTORY GAME ========================
int create_history_table() {
    char *sql = "CREATE TABLE IF NOT EXISTS history(" \
        "id INTEGER PRIMARY KEY AUTOINCREMENT," \
        "username TEXT NOT NULL," \
        "winner TEXT NOT NULL," \
        "loser TEXT NOT NULL," \
        "time TEXT NOT NULL);";
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    else {
        fprintf(stdout, "Table history created successfully\n");
    }
    return 0;
}

int insert_history(char *username, char *winner, char *loser, char *time) {
    char *sql = "INSERT INTO history (username, winner, loser, time) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, winner, strlen(winner), 0);
    sqlite3_bind_text(stmt, 3, loser, strlen(loser), 0);
    sqlite3_bind_text(stmt, 4, time, strlen(time), 0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stdout, "Insert history successfully\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int get_history(char *username, char *history) {
    char *sql = "SELECT * FROM history WHERE username like ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int count = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    printf("username in sql: %s\n", username);

    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);

    while (1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char *username = (char *)sqlite3_column_text(stmt, 1);
            char *winner = (char *)sqlite3_column_text(stmt, 2);
            char *loser = (char *)sqlite3_column_text(stmt, 3);
            char *time = (char *)sqlite3_column_text(stmt, 4);
            
            char *history_item = (char *)malloc(1024);
            sprintf(history_item, "%d:%s:%s:%s:%s;", id, username, winner, loser, time);
            strcat(history, history_item);

            count++;
        }
        else {
            break;
        }
    }

    printf("history in database: %s\n", history);
    // merge all information into a tring with format: id:username:password:status;id:username:password:status;...
    sqlite3_finalize(stmt);
    return 0;
}