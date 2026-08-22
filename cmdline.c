#include "cmdline.h"
#include "appstate.h"
#include "tags.h"
#include <string.h>

char cmd_buf[CMD_MAX] = {0};
int cmd_len = 0;

// function pointer
//      void (*variable)(int var_a, int var_b)
//      add typedef to make it reusable
typedef void (*CommandHandler)(const char *arg);

static void cmd_ta(const char *arg) { tags_add_to_buf(arg); }
static void cmd_tca(const char *arg) { (void) arg; tags_clear_buf(); }
static void cmd_q(const char *arg) { (void) arg; running = 0; }

typedef struct {
    const char* name;
    CommandHandler handler;
} Command;

static const Command commands[] = {
    { "ta",     cmd_ta },
    { "tca",    cmd_tca },
    { "q",      cmd_q},
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void cmd_parse(const char *input, char *verb, size_t verb_size, char **arg_out) {
    while (*input == ' ') input++; // skip leading whitespace

    size_t i = 0;
    while (input[i] != '\0' && input[i] != ' ' && i < verb_size - 1) {
        verb[i] = input[i];
        i++;
    }
    verb[i] = '\0';

    const char *rest = input + i;
    while (*rest == ' ') rest++;
    
    *arg_out = (char *) rest;
}

void cmdline_execute(void){
    char verb[CMD_VERB_MAX] = {0};
    char *arg;
    cmd_parse(cmd_buf, verb, sizeof(verb), &arg);

    int found = 0;
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(verb, commands[i].name) == 0) {
            commands[i].handler(arg);
            found = 1;
            break;
        }
    }
    if (!found) {
        status_set("Unknown command: %s", verb);
    }

    cmd_len = 0;
    cmd_buf[0] = '\0';
    focus = FOCUS_INPUT;
}
