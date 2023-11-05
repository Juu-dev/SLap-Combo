#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_HOST "localhost"
#define DB_USER "your_db_username"
#define DB_PASS "your_db_password"
#define DB_NAME "your_db_name"

static MYSQL *conn;
static int user_logged_in = 0;

int connect_to_db() {
    conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 1;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return 1;
    }

    return 0;
}

int register_user(const char *username, const char *password) {
    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')", username, password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Insert failed: %s\n", mysql_error(conn));
        return 1;
    }

    return 0;
}

int login_user(const char *username, const char *password) {
    char query[512];
    snprintf(query, sizeof(query), "SELECT username FROM users WHERE username='%s' AND password='%s'", username, password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Login failed: %s\n", mysql_error(conn));
        return 1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "No result returned.\n");
        return 1;
    }

    if (mysql_num_rows(result) == 1) {
        user_logged_in = 1;
        return 0;
    } else {
        fprintf(stderr, "Invalid login credentials.\n");
        return 1;
    }
}

int logout_user() {
    user_logged_in = 0;
    // You may perform additional cleanup here if needed
    return 0;
}