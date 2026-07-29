#ifndef TERMINAL_H
#define TERMINAL_H

#include <signal.h>

typedef struct {
    int rows;
    int cols;
} TermSize;

extern TermSize term_size;
extern volatile sig_atomic_t terminal_needs_redraw;

void terminal_update(void); 
void terminal_handle_winch(int sig);
void terminal_init(void);
void terminal_enable_raw_mode(void);
void terminal_disable_raw_mode(void);
void terminal_restore_screen(void);

#endif
