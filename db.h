#ifndef DB_H
#define DB_H
#include <sqlite3.h>

int db_init(const char *db_path);
void db_close(void);
sqlite3 *db_handle(void); // for other modules to borrow connection


#endif
