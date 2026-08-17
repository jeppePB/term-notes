#ifndef INPUT_H
#define INPUT_H
// Input config
#include "termbox2.h"
#define INPUT_MAX 256
#define CMD_MAX 32

extern char cmd_buf[CMD_MAX];
extern int cmd_len;

extern char input_buf[INPUT_MAX];
extern int input_len;

extern int input_last_key;
int input_process_event(struct tb_event *ev);
#endif
