#include "db.h"
#include <stdio.h>

static sqlite3 *db = NULL;

int db_init(const char *db_path){
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0;
}

void db_close(void){
    if (db) sqlite3_close(db);
}

sqlite3 *db_handle(void){
    return db;
}
