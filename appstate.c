#include <stdio.h>
#include "appstate.h"

// ui
FocusMode focus = FOCUS_INPUT;
int scroll_offset = 0;

//program state
int running = 1;

//errors
char status_message[STATUS_MSG_MAX_LEN] = {0};
int status_displayed = 1;

//tags
char active_tags[MAX_ACTIVE_TAGS][TAG_NAME_MAX_LEN];
int active_tag_count = 0;

void status_set(const char *msg){
    snprintf(status_message, STATUS_MSG_MAX_LEN, "%s", msg);
    status_displayed = 0;
}
