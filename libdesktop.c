#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libdesktop.h"

/* XXX We aren't currently handling multilingual .desktop files at all.*/

char *dynamic_fgets(char *line, size_t *size, FILE *fp) {
	int length = 0;
	if(!line) {
		*size = 255;
		line = malloc(*size * sizeof(*line));
		line[0] = '\0';
	}
	do {
		if(length > 0) {
			*size *= 2;
			if(!(line = realloc(line, *size))) {
				*size = 0;
				return NULL;
			}
		}
		if(!fgets((line+length), *size, fp)) {
			free(line);
			return NULL;
		}
		length += strlen(line+length);
	} while(line[length-1] != '\n');
	return line;
}

int desktop_read_file(struct DesktopEntry *d, FILE *fp) {
	#define KEY_TO_STRUCT(k) if(strcmp(line, #k) == 0) d->k = strdup(sep)
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
		KEY_TO_STRUCT(Name);
		else
		KEY_TO_STRUCT(GenericName);
		else
		KEY_TO_STRUCT(TryExec);
		else
		KEY_TO_STRUCT(Exec);
		else
		KEY_TO_STRUCT(Path);
		else
		KEY_TO_STRUCT(Icon);
	}
	if(size == 0) {
		return -1;
	}
	return 0;
}

int desktop_exec(struct DesktopEntry entry, int (*cb)(const char *)) {
	size_t length = strlen(entry.Exec);
	size_t size = length * 2;
	char *s = malloc(length * 2 * sizeof(*s));
	int is_symbol = 0;
	unsigned int i, c = 0;

	s[0] = '\0';

	for(i = 0; i < length; i++) {
		if(c+1 == size) {
			size *= 2;
			s = realloc(s, size * sizeof(*s));
		}
		if(is_symbol) {
			switch(entry.Exec[i]) {
				case '%':
					s[c++] = entry.Exec[i];
					break;
				case 'f':
					/* Not currently handling */
					break;
				case 'F':
					/* Not currently handling */
					break;
				case 'u':
					/* Not currently handling */
					break;
				case 'U':
					/* Not currently handling */
					break;
				case 'i':
					if(entry.Icon) {
						c += 9+strlen(entry.Icon);
						if(c > size) {
							size = (size+9+strlen(entry.Icon))*2;
							s = realloc(s, size * sizeof(*s));
						}
						strcat(s, "--icon '");
						strcat(s, entry.Icon);
						strcat(s, "'");
					}
					break;
				case 'c':
					if(entry.Name) {
						c += 2+strlen(entry.Name);
						if(c > size) {
							size = (size+2+strlen(entry.Name))*2;
							s = realloc(s, size * sizeof(*s));
						}
						strcat(s, "'");
						strcat(s, entry.Name);
						strcat(s, "'");
					}
					break;
				case 'k':
					if(entry._path) {
						c += 2+strlen(entry._path);
						if(c > size) {
							size = (size+2+strlen(entry._path))*2;
							s = realloc(s, size * sizeof(*s));
						}
						strcat(s, "'");
						strcat(s, entry._path);
						strcat(s, "'");
					}
					break;
				default:
					/* bad */;
			}
			is_symbol = 0;
		} else {
			if(entry.Exec[i] == '%') {
				is_symbol = 1;
			} else {
				s[c++] = entry.Exec[i];
			}
		}
		s[c] = '\0';
	}

	i = cb(s);
	free(s);
	return i;
}
