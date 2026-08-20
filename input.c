#include "termbox2.h"
#include "appstate.h"
#include "note_table.h"
#include "tags.h"
#include "cmdline.h"
#include <unistd.h>
#include "input.h"

char input_buf[INPUT_MAX] = {0};
int input_len = 0;
int input_last_key = -1;

static void input_process_command(struct tb_event *ev) {
    if (ev -> key == TB_KEY_ESC) {
        cmd_len = 0;
        cmd_buf[0] = '\0';
        focus = FOCUS_INPUT;
    } else if (ev -> key == TB_KEY_ENTER) {
        cmdline_execute();
    } else if (ev -> key == TB_KEY_BACKSPACE || ev -> key == TB_KEY_BACKSPACE2) {
        if (cmd_len > 0) {
            cmd_len--;
            cmd_buf[cmd_len] = '\0';
        }
        else {
            focus = FOCUS_INPUT;
        }
    } else if (ev -> ch != 0 && cmd_len < CMD_MAX -1) {
        cmd_buf[cmd_len++] = (char) ev-> ch;
        cmd_buf[cmd_len] = '\0';
    }
    return;
}

static void input_process_notes(struct tb_event *ev) {
    int note_count_total = note_total_count();
    if (ev -> key == TB_KEY_ARROW_UP || ev->ch =='k') {
        if (scroll_offset < note_count_total - 1) scroll_offset++;
    }
    else if (ev -> key == TB_KEY_ARROW_DOWN || ev->ch == 'j') {
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
    return;
}
static void input_process_input(struct tb_event *ev) {
    if (ev -> key == TB_KEY_BACKSPACE || ev -> key == TB_KEY_BACKSPACE2) {
        if (input_len > 0) {
            input_len--;
            input_buf[input_len] = '\0';
        }
    } 
    else if (ev -> key == TB_KEY_ENTER) {
        if (input_len > 0) {
            long long note_id = note_push(input_buf);
            if (note_id != -1) {
                for (int i = 0; i < active_tag_count; i++) {
                    long long tag_id = tags_get_or_create(active_tags[i]);
                    if (tag_id != -1) {
                        tags_attach_to_note(note_id, tag_id);
                    }
                }
                scroll_offset = 0; // Snap back to latest entry
            }
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

void input_process_event(struct tb_event *ev) {
    input_last_key = ev->ch;
    if (ev -> type != TB_EVENT_KEY) return;
    if (ev -> key == TB_KEY_CTRL_Q) running = 0; // Quit regardless of focus

    if (ev -> key == TB_KEY_TAB) {
        focus = (focus == FOCUS_INPUT) ? FOCUS_NOTES : FOCUS_INPUT;
        return;
    }    
    if (focus == FOCUS_INPUT && ev->ch == ':') {
        focus = FOCUS_COMMAND;
        return;
    }
    if (focus == FOCUS_COMMAND) input_process_command(ev);
    else if (focus == FOCUS_NOTES) input_process_notes(ev);
    else if (focus == FOCUS_INPUT) input_process_input(ev);

    return;
}
