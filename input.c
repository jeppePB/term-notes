#include "termbox2.h"
#include "appstate.h"
#include "note_table.h"
#include <unistd.h>
#include "input.h"

char input_buf[INPUT_MAX] = {0};
int input_len = 0;
int input_last_key = -1;

int input_process_event(struct tb_event *ev) {
    input_last_key = ev->ch;
    if (ev -> type != TB_EVENT_KEY) return 1;
    if (ev -> key == TB_KEY_CTRL_Q) return 0; // Quit regardless of focus

    if (ev -> key == TB_KEY_TAB) {
        focus = (focus == FOCUS_INPUT) ? FOCUS_NOTES : FOCUS_INPUT;
        return 1;
    }    
    if (focus == FOCUS_NOTES) {
        int note_count_total = note_total_count();
        if (ev -> key == TB_KEY_ARROW_UP) {
            if (scroll_offset < note_count_total - 1) scroll_offset++;
        }
        else if (ev -> key == TB_KEY_ARROW_DOWN) {
            if (scroll_offset > 0) scroll_offset--;
        }
        else if (ev -> key == TB_KEY_PGUP) {
            scroll_offset+=10;
            if (scroll_offset > note_count_total - 1) scroll_offset = note_count_total - 1;
        }
        else if (ev -> key == TB_KEY_PGDN) {
            scroll_offset-=10;
            if (scroll_offset < 0) scroll_offset = 0;
        }
        return 1;
    }
    if (focus == FOCUS_INPUT) {
        if (ev -> key == TB_KEY_BACKSPACE || ev -> key == TB_KEY_BACKSPACE2) {
            if (input_len > 0) {
                input_len--;
                input_buf[input_len] = '\0';
            }
        } 
        else if (ev -> key == TB_KEY_ENTER) {
           if (input_len > 0) {
               note_push(input_buf);
               scroll_offset = 0; // Snap back to latest entry
           }
           input_len = 0;
           input_buf[0] = '\0';
        }
        else if (ev -> ch != 0) {
            if (input_len < INPUT_MAX - 1){
                input_buf[input_len++] = (char) ev -> ch;
                input_buf[input_len] = '\0';
            }
        }
    }
    return 1;
}
