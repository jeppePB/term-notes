#include "input.h"
#include "ui.h"
#include "termbox2.h"
#include "db.h"
#include "note_table.h"

int main(void) {
    db_init("test.db");
    note_init();

    struct tb_event ev;

    if (tb_init() != 0) {
        fprintf(stderr, "Failed to initialize termbox\n");
        return 1;
    }        

    int running = 1;
    while (running) {
        tb_clear();
        ui_draw_screen();
        tb_present();
        tb_poll_event(&ev);
        running = input_process_event(&ev);
    }
    db_close();
    tb_shutdown();

   return 0;
}
