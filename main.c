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

   return 0;
}
