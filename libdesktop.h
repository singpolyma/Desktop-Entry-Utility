#ifndef _LIBDESKTOP_H
#define _LIBDESKTOP_H

struct DesktopEntry {
	char *Name;
	char *GenericName;
	char *TryExec;
	char *Exec;
	char *Path;
	char *Icon;
	char *_path;
};

int desktop_read_file(struct DesktopEntry *d, FILE *fp);
int desktop_exec(struct DesktopEntry entry, int (*cb)(const char *));

#endif
