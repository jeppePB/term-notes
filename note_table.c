#include <stdio.h>
#include <string.h>
#include "note_table.h"

Note notes[NOTE_BUF_MAX_LEN];

int note_count = 0;

void note_push(const char *line) {
    Note new_note;
    new_note.timestamp = time(NULL);
    snprintf(new_note.content, NOTE_CONTENT_MAX_LEN, "%s", line);

    if (note_count < NOTE_BUF_MAX_LEN) {
        notes[note_count++] = new_note;
    } else {
        for (int i = 1; i < NOTE_BUF_MAX_LEN; i++) {
            notes[i-1] = notes[i];
        }
        notes[NOTE_BUF_MAX_LEN - 1] = new_note;
    }
}      


