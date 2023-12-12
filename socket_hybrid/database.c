#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *db;

int initialize_database();
int create_users_table();
int create_new_user(char *username, char *password);
int update_status(char *username, char *status);
int check_username(char *username);
int check_password(char *username, char *password);

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
        "status TEXT NOT NULL DEFAULT 'offline');";
    
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