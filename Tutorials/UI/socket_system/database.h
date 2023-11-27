#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

sqlite3 *db;

int initialize_database() {
    // Open database
    int rc = sqlite3_open("user_database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    // Create table
    char *sql = "CREATE TABLE IF NOT EXISTS USERS("  \
                "USERNAME TEXT PRIMARY KEY NOT NULL," \
                "PASSWORD TEXT NOT NULL," \
                "HEALTH TEXT NULL," \
                "ATTACK TEXT NULL," \
                "LEVEL TEXT NULL," \
                "SOCKET TEXT NULL," \
                "STATUS TEXT NULL);";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    } else {
        fprintf(stdout, "USERS Table created successfully\n");
    }
    return 0;
}

#endif