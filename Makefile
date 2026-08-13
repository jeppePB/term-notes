CC = clang
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -g
LDLIBS = -lsqlite3

TARGET = app

SRCS = main.c appconfig.c appstate.c db.c note_table.c input.c ui.c termbox_impl.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
