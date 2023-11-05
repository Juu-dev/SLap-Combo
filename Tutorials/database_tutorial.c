#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Khởi tạo kết nối
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 1;
    }

    // Kết nối đến cơ sở dữ liệu
    if (mysql_real_connect(conn, "localhost", "username", "password", "database_name", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return 1;
    }

    // Thực hiện truy vấn
    if (mysql_query(conn, "SELECT * FROM users")) {
        fprintf(stderr, "SELECT * FROM users failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed\n");
        mysql_close(conn);
        return 1;
    }

    // In kết quả truy vấn
    while ((row = mysql_fetch_row(res))) {
        printf("User ID: %s, Username: %s\n", row[0], row[1]);
    }

    // Giải phóng bộ nhớ và đóng kết nối
    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}
