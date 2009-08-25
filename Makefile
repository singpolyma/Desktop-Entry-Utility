CFLAGS=-g -Wall -Wextra -ansi -pedantic -D_XOPEN_SOURCE -D_BSD_SOURCE

desktop: desktop.c libdesktop.c

.PHONY: clean

clean:
	$(RM) desktop
