#ifndef TAGS_H
#define TAGS_H

int tags_init(void);
long long tags_get_or_create(const char *name); // returns existing id or creates and returns new id
int tags_attach_to_note(long long note_id, long long tag_id);

#endif
