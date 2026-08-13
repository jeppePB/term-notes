#include <time.h>
#include "appstate.h"
#include "appconfig.h"
#include "note_table.h"
#include "input.h"
#include "termbox2.h"

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
static void fill_row(int row, uintattr_t color_bg) {
    int width = tb_width();
    for (int x = 0; x < width; x++) {
        tb_set_cell(x, row, ' ', TB_WHITE, color_bg);
    }
}
static void fill_region(int top_row, int bottom_row, uintattr_t color_bg) {
    for (int i = top_row; i < bottom_row; i++) {
        fill_row(i, color_bg);
    }
}

static void ui_draw_notes(int top_row, int bottom_row) {
    int visible = bottom_row - top_row;
    if (visible < 1) return;

    int pane_width = tb_width();
    char wrapped[MAX_LINE_WRAPS][MAX_INPUT];
    int row = bottom_row - 1; // Fill from bottom upward
    int idx = note_count - 1 - scroll_offset;

    while (idx >= 0 && row >= top_row) {
        struct tm local_time;
        localtime_r(&notes[idx].timestamp, &local_time);
        char timestamp_str[TIMESTAMP_COL_WIDTH];
        strftime(timestamp_str, TIMESTAMP_COL_WIDTH, "%m-%d %H:%M", &local_time);

        int wrapped_count = wrap_text(notes[idx].content, pane_width - TIMESTAMP_COL_WIDTH, wrapped, MAX_LINE_WRAPS);

        for (int i = wrapped_count - 1; i >= 0 && row >= top_row; i--){
            tb_print(TIMESTAMP_COL_WIDTH, row, TB_WHITE, TB_DEFAULT, wrapped[i]);
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

static void ui_draw_input_widget(int top_row, int bottom_row) {
    fill_region(top_row, bottom_row, color_input_bg);
    char wrapped[MAX_LINE_WRAPS][MAX_INPUT];
    int lines = wrap_text(input_buf, tb_width(), wrapped, MAX_LINE_WRAPS); 
    for (int i = 0; i < lines; i++) {
        tb_print(0, top_row+i, 0, color_input_bg, wrapped[i]);
    }
    if (focus == FOCUS_INPUT) {
        int c_x = 0;
        int c_y = top_row;
        if (lines > 0) {
            c_x = strlen(wrapped[lines-1]);
            c_y = top_row + lines - 1;
        }
        tb_set_cursor(c_x, c_y);
    } else {
        tb_hide_cursor();
    }
}

void ui_draw_screen(void) {
    char header[] = "-- type something, Enter to submit, ctrl-q to quit --";
    char debug[] = "[debug] input_height: %d | charcount: %d | term_size: %d rows %d cols";
    int y = 0;

    int rows_needed = (MAX_INPUT + tb_width() - 1) / tb_width(); // ceiling division
    int input_widget_height = (rows_needed > MAX_LINE_WRAPS) ? MAX_LINE_WRAPS : rows_needed;
    int input_widget_y = tb_height() - input_widget_height - 1; // one row reserved for debug line

    tb_print(0, y++, TB_WHITE, TB_DEFAULT, header); 
    ui_draw_notes(y, input_widget_y-1);
    ui_draw_input_widget(input_widget_y, input_widget_y + input_widget_height);
    tb_printf(0, tb_height() - 1, 0, 0, debug, input_widget_height, input_len, tb_height(), tb_width()); 
}
