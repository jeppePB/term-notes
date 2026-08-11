#ifndef APPSTATE_H
#define APPSTATE_H

typedef enum {
    FOCUS_INPUT,
    FOCUS_NOTES
} FocusMode;

extern FocusMode focus;
extern int scroll_offset;

#endif
