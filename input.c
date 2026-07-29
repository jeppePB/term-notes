#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "input.h"
#include "terminal.h"

char input_buf[INPUT_MAX] = {0};
int input_len = 0;
int input_last_key = -1;

int input_read_key(void) {
    unsigned char c;
    int nread;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if (nread == -1 && errno != EINTR) exit(1); // read is -1 and read was not interrupted by signal
        if (terminal_needs_redraw) {
            terminal_needs_redraw = 0;
            return KEY_RESIZE;
        }
    }
    

    // If c is an ordinary keypress, return as-is
    if (c != '\x1b') {
        return c;
    }

    // Handle CSI sequence
    char bracket;
    if (read(STDIN_FILENO, &bracket, 1) != 1) return '\x1b';
    if (bracket != '[') return '\x1b';

    char params[16];
    int plen = 0;
    char final = 0;

    while (1) {
        char b;
        if (read(STDIN_FILENO, &b, 1) != 1) return '\x1b';
        
        if ((b >= '0' && b <= '9') || b == ';') {
            if (plen < (int)sizeof(params) - 1) params[plen++] = b;
            continue;
        }
        final = b;
        break;
    }
    params[plen] = '\0';

    if (plen == 0) {
        switch (final) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
        }
    } else if (final == '~') {
        int code = atoi(params);
        switch (code) {
            case 1: return HOME_KEY;
            case 3: return DEL_KEY;
            case 4: return END_KEY;
            case 5: return PAGE_UP;
            case 6: return PAGE_DOWN;
        }
    }
 
    return '\x1b';
}


