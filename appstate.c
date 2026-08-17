#include "appstate.h"

FocusMode focus = FOCUS_INPUT;
int scroll_offset = 0;

char active_tags[MAX_ACTIVE_TAGS][TAG_NAME_MAX_LEN];
int active_tag_count = 0;
