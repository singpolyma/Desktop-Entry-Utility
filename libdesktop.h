#ifndef _LIBDESKTOP_H
#define _LIBDESKTOP_H

struct DesktopEntry {
	char *Name;
	char *GenericName;
	char *TryExec;
	char *Exec;
	char *Folder;
	char *Icon;
};

int desktop_read_file(struct DesktopEntry *d, FILE *fp);
void desktop_exec(struct DesktopEntry entry, int (*cb)(const char *));

#endif
