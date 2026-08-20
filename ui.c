#include <time.h>
#include "appstate.h"
#include "appconfig.h"
#include "note_table.h"
#include "input.h"
#include "cmdline.h"
#include "termbox2.h"

typedef struct {
    int top, bottom;
    int x_cursor, y_cursor;
    int is_focused;
} Widget;

// returns number of wrapped lines written. wrapped segments are written to out[][].
// each out[i] is a buffer of at least MAX_INPUT + 1 chars.
static int ui_wrap_text(const char* text, int max_line_len, char out[][MAX_INPUT], int max_out_lines) {
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
static void ui_draw_focus_indicator(Widget *w) {
    uintattr_t color = w->is_focused ? color_focus_indicator : TB_DEFAULT;
    for (int row = w->top; row <= w->bottom; row++) {
        tb_set_cell(w->x_cursor, row, ' ', TB_DEFAULT, color);
    }
    w->x_cursor++; // indicator + padding
}

static void ui_fill_row(int row, uintattr_t color_bg) {
    int width = tb_width();
    for (int x = 0; x < width; x++) {
        tb_set_cell(x, row, ' ', TB_WHITE, color_bg);
    }
}
static void ui_fill_region(int top_row, int bottom_row, uintattr_t color_bg) {
    for (int i = top_row; i < bottom_row; i++) {
        ui_fill_row(i, color_bg);
    }
}

static void ui_draw_notes(Widget *w) {
    int padding = 1;
    ui_draw_focus_indicator(w);
    if (w->is_focused) { tb_hide_cursor(); }

    w->x_cursor += padding;

    Note recent[64];
    int n = note_get_recent_filtered(recent, 64);

    // hide note widget if too few rows are available
    int visible = w->bottom - w->top;
    if (visible < 1) return;

    int pane_width = tb_width();
    char wrapped[MAX_LINE_WRAPS][MAX_INPUT];
    int row = w->bottom; // fill from bottom upward
    int idx = 0 + scroll_offset;

    while (idx < n && row >= w->top) {
        struct tm local_time;
        localtime_r(&recent[idx].timestamp, &local_time);
        char timestamp_str[TIMESTAMP_COL_WIDTH];
        strftime(timestamp_str, TIMESTAMP_COL_WIDTH, "%m-%d %H:%M", &local_time);

        int wrapped_count = ui_wrap_text(recent[idx].content, pane_width - TIMESTAMP_COL_WIDTH, wrapped, MAX_LINE_WRAPS);
        
        // draw row
        for (int i = wrapped_count - 1; i >= 0 && row >= w->top; i--) {
            tb_print(TIMESTAMP_COL_WIDTH + w->x_cursor, row, TB_WHITE, TB_DEFAULT, wrapped[i]);
            if (i == 0) {
                tb_print(w->x_cursor, row, TB_YELLOW, TB_BLACK, timestamp_str);
            } else if (row == w->top) {
                tb_print(w->x_cursor + 5, row, TB_YELLOW, TB_BLACK, "^"); // 'magic' number to put the '^' in the center of the datestring
            }
            row--;
        }
        idx++;
    }
}

static void ui_draw_input_widget(Widget *w) {
    ui_fill_region(w->top, w->bottom, color_input_bg);
    char wrapped[MAX_LINE_WRAPS][MAX_INPUT];
    int lines = ui_wrap_text(input_buf, tb_width(), wrapped, MAX_LINE_WRAPS); 
    
 
    for (int i = 0; i < lines; i++) {
        tb_print(0, w->top+i, 0, color_input_bg, wrapped[i]);
        w->y_cursor = i;
        w->x_cursor = strlen(wrapped[i]);
    }
    if (w->is_focused) {
        tb_set_cursor(w->x_cursor, w->top + w->y_cursor);
    }
}

static void ui_draw_cmd(Widget *w) {
    w->y_cursor = w->top;
    if (w->is_focused) {
        status_displayed = 1;
        tb_print(w->x_cursor++, w->y_cursor, TB_YELLOW, TB_DEFAULT, ":");
        tb_print(w->x_cursor, w->y_cursor, TB_YELLOW, TB_DEFAULT, cmd_buf);
        w->x_cursor += strlen(cmd_buf);
        tb_set_cursor(w->x_cursor, w->y_cursor);
    } else if (!status_displayed) {
        tb_print(w->x_cursor, w->y_cursor, TB_RED, TB_DEFAULT, status_message);
    }
}

static void ui_draw_tags(Widget *w) {
    for (int i = 0; i < active_tag_count; i++) {
        tb_print(w->x_cursor, w->top, TB_BLUE, TB_BLACK, active_tags[i]);
        w->x_cursor += strlen(active_tags[i]);
        w->x_cursor += 1;
    } 
}
static void draw_debug(int row) {
    char debug[] = "[debug] charcount: %d | term_size: %d rows %d cols";
    tb_printf(0, tb_height() - 1, 0, 0, debug, input_len, tb_height(), tb_width()); 
}

static void ui_draw_error_screen() {
    ui_fill_region(0, tb_height(), TB_DEFAULT);
    char err_msg[] = "Bad dimensions.\n Min rows: %d\n Min cols: %d\n Actual rows: %d\n Actual cols: %d\n";
    tb_printf(0, 0, TB_RED, TB_BLACK, err_msg, MIN_TERM_HEIGHT, MIN_TERM_WIDTH, tb_height(), tb_width());
}
static int ui_check_terminal_sufficient() {
    if ((tb_height() < MIN_TERM_HEIGHT) || (tb_width() < MIN_TERM_WIDTH)) {
        return 0;
    }
    return 1;
}

void ui_draw_screen(void) {
    if (!ui_check_terminal_sufficient()) {
        ui_draw_error_screen();
        return;
    }

    char header[] = "-- type something, Enter to submit, ctrl-q to quit --";
    int y = 0;

    int input_rows_needed = (MAX_INPUT + tb_width() - 1) / tb_width(); // ceiling division

    int input_widget_height = (input_rows_needed > MAX_LINE_WRAPS) ? MAX_LINE_WRAPS : input_rows_needed;
    int input_widget_bottom = tb_height() - 1; // one row reserved for debug line
    int input_widget_top = input_widget_bottom - input_widget_height;
    tb_print(0, y++, TB_WHITE, TB_DEFAULT, header); 
    Widget notes_widget = {
        .top = y, 
        .bottom = input_widget_top - 2,
        .is_focused = (focus == FOCUS_NOTES)
    };
    Widget tags_widget = {
        .top = input_widget_top-1,
        .bottom = input_widget_top-1,
    };
    Widget input_widget = {
        .top = input_widget_top,
        .bottom = input_widget_bottom,
        .is_focused = (focus == FOCUS_INPUT)
    };
    Widget cmd_widget = {
        .top = input_widget_bottom,
        .bottom = input_widget_bottom,
        .is_focused = (focus == FOCUS_COMMAND)
    };
    
    ui_draw_notes(&notes_widget);
    ui_draw_tags(&tags_widget);
    ui_draw_input_widget(&input_widget);
    ui_draw_cmd(&cmd_widget);
}
