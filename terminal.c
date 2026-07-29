#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include "terminal.h"

// window config
#define DEFAULT_TERM_ROWS 28
#define DEFAULT_TERM_COLS 80

struct termios orig_termios;
TermSize term_size = {.rows = DEFAULT_TERM_ROWS, .cols = DEFAULT_TERM_COLS};
volatile sig_atomic_t terminal_needs_redraw = 0;

void terminal_update(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_row == 0){
        // ioctl failed or returned bad values
        return;
    }
    term_size.rows = ws.ws_row;
    term_size.cols = ws.ws_col;
}

void terminal_handle_winch(int sig) {
    (void)sig;  // unused parameter - silences -Wunused warnings
    terminal_update();
    terminal_needs_redraw = 1;
}

void terminal_init(void) {
    terminal_update();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = terminal_handle_winch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGWINCH, &sa, NULL);
}

// SIGNALS - 0 is stop loop, 1 is keep going

void terminal_disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void terminal_restore_screen(void) {
    write(STDOUT_FILENO, "\x1b[?1049l", 8); // Leave the alternate screen
}

void terminal_enable_raw_mode(void) {
    // Get the settings of the current terminal session, store in orig_termios struct
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(terminal_disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON | ICRNL);     // IXON intercepts control bytes, icnrl translates \r into \n
    raw.c_cc[VMIN] = 0;     // Set read() to return even if 0 bytes read
    raw.c_cc[VTIME] = 1;    // wait for up to 100ms for input
    // Set terminal session settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


