#ifndef NOTE_TABLE_H
#define NOTE_TABLE_H

#include <time.h>

// Log config
#define NOTE_CONTENT_MAX_LEN 256    // max chars in line

typedef struct {
    long long id;
    time_t timestamp;
    char content[NOTE_CONTENT_MAX_LEN];
} Note;

int note_init(void);
long long note_push(const char *line);

int note_get_recent(Note *out, int max_count);
int note_get_recent_filtered(Note *out, int max_count);
int note_total_count(void);

#endif
