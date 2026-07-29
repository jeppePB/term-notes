#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "terminal.h"
#include "note_table.h"
#include "input.h"
#include "ui.h"

int process_keypress(void) {
    int c = input_read_key();
    input_last_key = c;

    if (c == KEY_RESIZE) {
        return 1;
    }
    if (c == 17) { // ctrl + q to quit
        return 0;
    } else if (c == 127 || c == 8) { // backspace (usually 128, sometimes 8)
        if (input_len > 0) {
            input_len--;
            input_buf[input_len] = '\0';
        }
    } else if (c == '\r' || c == '\n') {
        // Enter - submit to log (not implemented)
        if (input_len > 0) {
            note_push(input_buf);
        }
        input_len = 0;
        input_buf[0] = '\0';
    } else if (c  >= 32 && c < 127) { // normal chars
        if (input_len < INPUT_MAX - 1) {
            input_buf[input_len++] = (char)c;
            input_buf[input_len] = '\0';
        }
    }
    return 1;
}

int main(void) {
    
    write(STDOUT_FILENO, "\x1b[?1049h", 8);  // enter alternate screen
    atexit(terminal_restore_screen);

    terminal_enable_raw_mode();
    terminal_init();

    ui_draw_screen();
    while(process_keypress()) {
        ui_draw_screen();
    }
    return 0;
}
