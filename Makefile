CFLAGS=-g -Wall -Wextra -ansi -pedantic -D_XOPEN_SOURCE -D_BSD_SOURCE

desktop-entry-util: desktop-entry-util.c libdesktop.c

.PHONY: clean

clean:
	$(RM) desktop-entry-util
