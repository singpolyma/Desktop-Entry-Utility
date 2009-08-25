#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libdesktop.h"

char *dynamic_fgets(char *line, size_t *size, FILE *fp) {
	int length = 0;
	if(!line) {
		*size = 255;
		line = malloc(*size * sizeof(*line));
		line[0] = '\0';
	}
	while(line[length-1] != '\n') {
		if(length > 0) {
			*size *= 2;
			if(!(line = realloc(line, *size))) {
				*size = -1;
				return NULL;
			}
		}
		if(!fgets((line+length), *size, fp)) {
			return NULL;
		}
		length += strlen(line+length);
	}
	return line;
}

int desktop_read_file(struct DesktopEntry *d, FILE *fp) {
	#define KEY_TO_STRUCT(k) if(strcmp(line, #k) == 0) d->k = strdup(sep);
	char *line = NULL;
	char *sep = NULL;
	size_t size = 0;
	while((line = dynamic_fgets(line, &size, fp))) {
		if(line[0] == '#') {
			continue; /* Comment, skip this line */
		}
		line[strlen(line)-1] = '\0'; /* chop */
		if((sep = strchr(line, '='))) {
			*sep = '\0';
			sep++;
		} else {
			continue; /* No =, skip this line */
		}
		KEY_TO_STRUCT(Name)
		else
		KEY_TO_STRUCT(GenericName)
		else
		KEY_TO_STRUCT(TryExec)
		else
		KEY_TO_STRUCT(Exec)
		else
		KEY_TO_STRUCT(Folder)
		else
		KEY_TO_STRUCT(Icon)
	}
	if(size == -1) {
		return -1;
	}
	return 0;
}

void desktop_exec(struct DesktopEntry entry, int (*cb)(const char *)) {
	cb(entry.Exec);
}

