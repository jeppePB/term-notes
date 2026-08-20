#ifndef TAGS_H
#define TAGS_H

int tags_init(void);
long long tags_get_or_create(const char *name); // returns existing id or creates and returns new id
int tags_attach_to_note(long long note_id, long long tag_id);
void tags_add_to_buf(const char *name);
void tags_clear_buf();
#endif
