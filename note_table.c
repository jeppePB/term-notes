#include <stdio.h>
#include <string.h>
#include "note_table.h"
#include "db.h"
#include "appstate.h"

int note_init(void) {
    const char *create_sql = 
        "CREATE TABLE IF NOT EXISTS notes ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp INTEGER NOT NULL,"
        "  content TEXT NOT NULL"
        ");";

    char *err_msg = NULL;
    int rc = sqlite3_exec(db_handle(), create_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

 long long note_push(const char *line) {
    const char *sql = "INSERT INTO notes (timestamp, content) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db_handle(), sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db_handle()));
        return -1;
    }

    sqlite3_bind_int64(stmt, 1, (long long)time(NULL));
    sqlite3_bind_text(stmt, 2, line, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db_handle()));
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db_handle());
}      

int note_get_recent(Note *out, int max_count) {
    const char *sql =
        "SELECT id, timestamp, content FROM notes "
        "ORDER BY id DESC LIMIT ?;";
    sqlite3 *db = db_handle();
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, max_count);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        out[count].id = sqlite3_column_int64(stmt, 0);
        out[count].timestamp = (time_t)sqlite3_column_int64(stmt, 1);
        
        const unsigned char *text = sqlite3_column_text(stmt, 2);
        snprintf(out[count].content, NOTE_CONTENT_MAX_LEN, "%s", text ? (const char *)text : "");

        count++;
    }
    sqlite3_finalize(stmt);
    return count;
}

int note_get_recent_filtered(Note *out, int max_count) {
    if (active_tag_count == 0) {
        return note_get_recent(out, max_count);
    }

    char sql[512];
    char placeholders[MAX_ACTIVE_TAGS * 2] = {0}; // "?, ?, ?" etc.
    for (int i = 0; i < active_tag_count; i++) {
        strcat(placeholders, i == 0 ? "?" : ",?");
    }
    
    snprintf(sql, sizeof(sql),
            "SELECT notes.id, notes.timestamp, notes.content "
            "FROM notes "
            "JOIN note_tags ON notes.id = note_tags.note_id "
            "JOIN tags ON tags.id = note_tags.tag_id "
            "WHERE tags.name IN (%s) "
            "GROUP BY notes.id "
            "HAVING COUNT(DISTINCT tags.name) = ? "
            "ORDER BY notes.id DESC LIMIT ?;", placeholders);

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_handle(), sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare (filtered notes) failed: %s\n", sqlite3_errmsg(db_handle()));
        return 0;
    }

    int col = 1;
    for (int i = 0; i < active_tag_count; i++) {
        sqlite3_bind_text(stmt, col++, active_tags[i], -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_int(stmt, col++, active_tag_count);    // HAVING COUNT(...) = ?
    sqlite3_bind_int(stmt, col, max_count);             // for LIMIT ?
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        out[count].id  = sqlite3_column_int64(stmt, 0);
        out[count].timestamp = (time_t)sqlite3_column_int64(stmt, 1);
        const unsigned char *text = sqlite3_column_text(stmt, 2);
        snprintf(out[count].content, NOTE_CONTENT_MAX_LEN, "%s", text ? (const char*)text : "");
        count++;
    }

    sqlite3_finalize(stmt);
    return count;
}

int note_total_count(void) {
    const char *sql = "SELECT COUNT(*) FROM notes;";
    sqlite3 *db = db_handle();
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}
