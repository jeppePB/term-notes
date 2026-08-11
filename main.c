#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "terminal.h"
#include "note_table.h"
#include "input.h"
#include "ui.h"
#include "termbox2.h"

int main(void) {
    struct tb_event ev;

    tb_init();        
    int exit_program = 1;
    while (exit_program) {
        tb_clear();
        ui_draw_screen();
        tb_present();
        tb_poll_event(&ev);
        exit_program = input_process_event(&ev);
    }
    tb_shutdown();

    // write(STDOUT_FILENO, "\x1b[?1049h", 8);  // enter alternate screen
    // atexit(terminal_restore_screen);
    //
    // terminal_enable_raw_mode();
    // terminal_init();
    //
    // ui_draw_screen();
    // while(process_keypress()) {
    //     ui_draw_screen();
    // }
    return 0;
}
