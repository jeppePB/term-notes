#ifndef CMDLINE_H
#define CMDLINE_H

#define CMD_MAX 128
#define CMD_VERB_MAX 16
extern char cmd_buf[CMD_MAX];
extern int cmd_len;

void cmdline_execute(void);

#endif
