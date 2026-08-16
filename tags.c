#include "tags.h"
#include "db.h"
#include <stdio.h>

int tags_init(void) {
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS tags ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE"
        ");"
        "CREATE TABLE IF NOT EXISTS note_tags ("
        " note_id INTEGER NOT NULL,"
        " tag_id INTEGER NOT NULL,"
        " PRIMARY KEY (note_id, tag_id)"
        ");";

    char *err_msg = NULL;
    int rc = sqlite3_exec(db_handle(), sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "tags_init failed: %s\n", err_msg);
        sqlite3_free(err_msg); 
        return -1;
    }
    return 0;
}

long long tags_get_or_create(const char *name) {
    // first check if tag exists, return existing id
    const char *select_sql = "SELECT id FROM tags WHERE name = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_handle(), select_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare (select tag) failed: %s\n", sqlite3_errmsg(db_handle()));
        return -1;
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    
    long long id = -1;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        id = sqlite3_column_int64(stmt, 0);
    } else if (rc != SQLITE_DONE) {
        // SQLITE_DONE just means no matching rows. Any other code is an error.
        fprintf(stderr, "Step (select tag) failed: %s\n", sqlite3_errmsg(db_handle()));
    }
    sqlite3_finalize(stmt);

    if (id != -1) return id;

    // tag not existing, create new and return that id
    const char *insert_sql = "INSERT INTO tags (name) VALUES (?);";
    rc = sqlite3_prepare_v2(db_handle(), insert_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf(stderr, "Prepare (insert tag) failed: %s\n", sqlite3_errmsg(db_handle()));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert tag failed: %s\n", sqlite3_errmsg(db_handle()));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    return sqlite3_last_insert_rowid(db_handle());
}

int tags_attach_to_note(long long note_id, long long tag_id) {
    const char *sql = "INSERT OR IGNORE INTO note_tags (note_id, tag_id) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_handle(), sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare (attach tag) failed: %s\n", sqlite3_errmsg(db_handle()));
        return -1;
    }
    sqlite3_bind_int64(stmt, 1, note_id);
    sqlite3_bind_int64(stmt, 2, tag_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Attach tag failed: %s\n", sqlite3_errmsg(db_handle()));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}
