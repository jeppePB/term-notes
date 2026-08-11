#include <time.h>
#include "appstate.h"
#include "note_table.h"
#include "input.h"

static void ui_draw_notes(int top_row, int bottom_row) {
    int visible = bottom_row - top_row;
    if (visible < 1) return;

    int end = note_count - scroll_offset;
    int start = end - visible;
    if (start < 0) start = 0;

    int row = top_row;
    
    for (int i = start; i < end; i++) {
        struct tm local_time;
        localtime_r(&notes[i].timestamp, &local_time);

        char timestamp_str[16];
        strftime(timestamp_str, sizeof(timestamp_str), "%m-%d %H:%M", &local_time);

        uintattr_t color = (focus == FOCUS_NOTES) ? TB_YELLOW : TB_WHITE;
        tb_printf(0, row, color, TB_DEFAULT, "[%s] %s", timestamp_str, notes[i].content);
        row++;
    }
    if (scroll_offset > 0) {
        tb_printf(bottom_row - visible, bottom_row + 1, TB_CYAN, TB_DEFAULT, 
                "-- scrolled back to %d --", scroll_offset);
    }
}

static void ui_draw_input_widget(int row) {
    char widget_str[] = "[no tag]";
    tb_printf(0, row, 0, 0, "%s %s", widget_str, input_buf);
}

void ui_draw_screen(void) {
    char header[] = "-- type something, Enter to submit, ctrl-q to quit --";
    char debug[] = "[debug] last key code: %d | charcount: %d | term_size: %d rows %d cols";
    int y = 0;
    tb_print(0, y++, TB_WHITE, TB_DEFAULT, header); 
    ui_draw_notes(y, tb_height() - 10);
    tb_printf(0, tb_height() - 3, 0, 0, "________________________________________________________________");
    ui_draw_input_widget(tb_height()-2);
    tb_printf(0, tb_height() - 1, 0, 0, debug, input_last_key, input_len, tb_height(), tb_width()); 
}
