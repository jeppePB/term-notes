#ifndef INPUT_H
#define INPUT_H
// Input config
#include "termbox2.h"
#define INPUT_MAX 256

extern char input_buf[INPUT_MAX];
extern int input_len;

extern int input_last_key;
int input_process_event(struct tb_event *ev);
#endif
