#ifndef INPUT_H
#define INPUT_H
// Input config
#define INPUT_MAX 256
enum input_key_action {
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    KEY_RESIZE // Not a real key, set by signal
};


extern char input_buf[INPUT_MAX];
extern int input_len;

extern int input_last_key;

int input_read_key(void);
#endif
