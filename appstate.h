#ifndef APPSTATE_H
#define APPSTATE_H

typedef enum {
    FOCUS_INPUT,
    FOCUS_NOTES,
    FOCUS_COMMAND
} FocusMode;

extern FocusMode focus;
extern int scroll_offset;

#define MAX_ACTIVE_TAGS 8
#define TAG_NAME_MAX_LEN 10

extern char active_tags[MAX_ACTIVE_TAGS][TAG_NAME_MAX_LEN];
extern int active_tag_count;

#endif
