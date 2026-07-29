#ifndef NOTE_TABLE_H
#define NOTE_TABLE_H

#include <time.h>

// Log config
#define NOTE_BUF_MAX_LEN 512             // lines
#define NOTE_CONTENT_MAX_LEN 256    // max chars in line

typedef struct {
    time_t timestamp;
    char content[NOTE_CONTENT_MAX_LEN];
} Note;

extern Note notes[NOTE_BUF_MAX_LEN];

extern int note_count;

void note_push(const char *line);

#endif
