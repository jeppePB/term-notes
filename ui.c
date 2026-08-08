#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "terminal.h"
#include "note_table.h"
#include "input.h"

static void ui_draw_notes(int start) {
    for (int i = start; i < note_count; i++) {
        struct tm local_time;
        localtime_r(&notes[i].timestamp, &local_time);
        
        char timestamp_str[16];
        strftime(timestamp_str, sizeof(timestamp_str), "%m-%d %H:%M", &local_time);

        char rendered[NOTE_CONTENT_MAX_LEN + 32];
        int len = snprintf(rendered, sizeof(rendered), "[%s] %s",
                    timestamp_str, notes[i].content);

        write(STDOUT_FILENO, rendered, len);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}

void ui_draw_screen(void) {
    write(STDOUT_FILENO, "\x1b[2J", 4);   // clear screen
    write(STDOUT_FILENO, "\x1b[H", 3);    // cursor to top-left}
    
    char header[] = "-- type something, Enter to submit, ctrl-q to quit --\r\n\r\n";
    write(STDOUT_FILENO, header, strlen(header));

    // reserve rows for header
    int reserved = 7;
    int visible_notes = term_size.rows - reserved;
    if (visible_notes < 1) visible_notes = 1;

    // display log lines
    int start = note_count > visible_notes ? note_count - visible_notes : 0;

    ui_draw_notes(start);
    write(STDOUT_FILENO, "\r\n>", 4);
    write(STDOUT_FILENO, input_buf, input_len);
    write(STDOUT_FILENO, "\r\n\r\n", 4);

    // debug view - raw numeric codes
    char debug_line[96];
    int len = snprintf(debug_line, sizeof(debug_line),
        "[debug] last key code: %d  |  charcount: %d | term size: %d rows %d cols %d redraw\r\n",
        input_last_key, input_len, term_size.rows, term_size.cols, terminal_needs_redraw);
    write(STDOUT_FILENO, debug_line, len);
}
