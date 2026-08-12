#include <linux/limits.h>
#include <time.h>
#include "appstate.h"
#include "note_table.h"
#include "input.h"

// returns number of wrapped lines written. wrapped segments are written to out[][].
// each out[i] is a buffer of at least MAX_INPUT + 1 chars.
static int wrap_text(const char* text, int max_line_len, char out[][MAX_INPUT], int max_out_lines) {
    int text_len = strlen(text);
    int out_count = 0;
    int pos = 0;

    while (pos < text_len && out_count < max_out_lines) {
        int remaining = text_len - pos;
        int take = (remaining < max_line_len) ? remaining : max_line_len;

        // try to split at space to avoid splitting mid-word
        if (take == max_line_len && pos + take < text_len) {
            int break_at = take;
            while (break_at > 0 && text[pos + break_at] != ' '){
                break_at--;
            }
            if (break_at > 0) take = break_at; // found a space to break on
            // if break_at == 0, the word is longer than max_line_len.
            // in that case, split the word
        }
        memcpy(out[out_count], text + pos, take);
        out[out_count][take] = '\0';
        out_count++;

        pos += take;
        while (pos < text_len && text[pos] == ' ') pos++;
    }

    return out_count;
}

static void ui_draw_notes(int top_row, int bottom_row) {
    int visible = bottom_row - top_row;
    if (visible < 1) return;

    int pane_width = tb_width();
    char wrapped[16][MAX_INPUT];
    int row = bottom_row - 1; // Fill from bottom upward
    int idx = note_count - 1 - scroll_offset;

    while (idx >= 0 && row >= top_row) {
        struct tm local_time;
        localtime_r(&notes[idx].timestamp, &local_time);
        char timestamp_str[12];
        strftime(timestamp_str, sizeof(timestamp_str), "%m-%d %H:%M", &local_time);

        int wrapped_count = wrap_text(notes[idx].content, pane_width - 12, wrapped, 16);

        for (int i = wrapped_count - 1; i >= 0 && row >= top_row; i--){
            tb_print(12, row, TB_WHITE, TB_DEFAULT, wrapped[i]);
            if (i == 0) {
                tb_print(0, row, TB_YELLOW, TB_BLACK, timestamp_str);
            } else if (row == top_row) {
                tb_print(6, row, TB_YELLOW, TB_BLACK, "^");
            }
            row --;
        }
        idx--;
    }
}


// static void ui_draw_notes(int top_row, int bottom_row) {
//     int visible = bottom_row - top_row;
//     if (visible < 1) return;
//
//     int end = note_count - scroll_offset;
//     int start = end - visible;
//     if (start < 0) start = 0;
//
//     int row = top_row;
//
//     for (int i = start; i < end; i++) {
//         struct tm local_time;
//         localtime_r(&notes[i].timestamp, &local_time);
//
//         char timestamp_str[16];
//         strftime(timestamp_str, sizeof(timestamp_str), "%m-%d %H:%M", &local_time);
//
//         uintattr_t color = (focus == FOCUS_NOTES) ? TB_YELLOW : TB_WHITE;
//         tb_printf(0, row, color, TB_DEFAULT, "[%s] %s", timestamp_str, notes[i].content);
//         row++;
//     }
//     if (scroll_offset > 0) {
//         tb_printf(bottom_row - visible, bottom_row + 1, TB_CYAN, TB_DEFAULT, 
//                 "-- scrolled back to %d --", scroll_offset);
//     }
// }
//
static void ui_draw_input_widget(int row) {
    char wrapped[16][MAX_INPUT];
    int lines = wrap_text(input_buf, tb_width(), wrapped, 16); 
    for (int i = 0; i < lines; i++) {
        tb_print(0, row+i, 0, 0, wrapped[i]);
    }
}

void ui_draw_screen(void) {
    char header[] = "-- type something, Enter to submit, ctrl-q to quit --";
    char debug[] = "[debug] last key code: %d | charcount: %d | term_size: %d rows %d cols";
    int y = 0;
    int input_widget_y = tb_height() - 8;
    tb_print(0, y++, TB_WHITE, TB_DEFAULT, header); 
    ui_draw_notes(y, input_widget_y-2);
    tb_printf(0, input_widget_y-1, 0, 0, "________________________________________________________________");
    ui_draw_input_widget(input_widget_y);
    tb_printf(0, tb_height() - 1, 0, 0, debug, input_last_key, input_len, tb_height(), tb_width()); 
}
