#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#if defined(_WIN32) || defined(__WIN32__)
	#include "getopt.h"
	#define ENV_SEPERATOR ";"

	#include <windows.h>
	char *realpath(const char *path, char *resolved_path) {
		if(!resolved_path) {
			resolved_path = malloc(PATH_MAX * sizeof(*resolved_path));
		}
		/* XXX If resolved_path was passed in and is too short, this is a bad idea. */
		if(GetFullPathName(path, PATH_MAX, resolved_path, NULL)) {
			return resolved_path;
		}
		return NULL;
	}

#else
	#include <unistd.h>
	#define ENV_SEPERATOR ":"
#endif

#include "libdesktop.h"

void help(char *name) {
	puts("Extract data from a .destkop (Desktop Entry) file.");
	puts("With no switches, execut the processed Exec line.");
	printf("Usage: %s [OPTION] [PATH]\n", name);
	puts("   PATH    Path to .desktop file");
	puts("   -h      help (this screen)");
	puts("   -o      output processed Exec line");
	puts("   -t      exit status EXIT_SUCCESS on TryExec exists, 2 on doesn't exist, EXIT_FAILURE on no such key");
	puts("   -i      output absolute path to icon");
	puts("   -n      output name");
	puts("   -g      output generic name");
}

FILE *xfopen(char *pth, char *mode) {
	FILE *fp;
	if(!(fp = fopen(pth, mode))) {
		fprintf(stderr, "Failed to open file '%s'\n", pth);
		exit(EXIT_FAILURE);
	}
	return fp;
}

int file_exists(const char *pth) {
	FILE *fp;
	fp = fopen(pth, "rb");
	if(fp) { /* The file exists */
		fclose(fp);
		return 1;
	}
	return 0;
}

int in_path(const char *pth) {
	char *env = strdup(getenv("PATH"));
	char *tmp = malloc(255 * sizeof(*tmp));
	char *tok = NULL;
	size_t size = 255;
	size_t pth_len = strlen(pth);
	if(env && tmp) {
		tok = strtok(env, ENV_SEPERATOR);
		while(tok) {
			if((tok = strtok(NULL, ENV_SEPERATOR))) {
				if(size < pth_len+strlen(tok)+2) {
					size *= 2;
					tmp = realloc(tmp, size*sizeof(*tmp));
				}
				strcpy(tmp, tok);
				strcat(tmp, "/");
				strcat(tmp, pth);
				if(file_exists(tmp)) {
					free(env);
					free(tmp);
					return 1;
				}
			}
		}
		free(env);
		free(tmp);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int c;
	char s = '\0';
	FILE *fp = NULL;
	struct DesktopEntry entry = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};

	while((c = getopt(argc, argv, "-hoting")) != -1) {
		switch(c) {
			case 'o':
			case 't':
			case 'i':
			case 'n':
			case 'g':
				if(s != '\0') {
					fprintf(stderr, "Cannot use '-%c' with '-%c'\n", c, s);
					help(argv[0]);
					exit(EXIT_FAILURE);
				}
				s = c;
				break;
			case '\1':
				if(fp) {
					fprintf(stderr, "Too many arguments: %s\n", optarg);
					help(argv[0]);
					exit(EXIT_FAILURE);
				}
				entry._path = strdup(optarg);
				fp = xfopen(optarg, "r");
				break;
			case 'h':
				help(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				help(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	/* On non-GNU systems, we won't have the arguments yet. */
	if(fp == NULL && optind < argc) {
		entry._path = strdup(argv[optind+1]);
		fp = xfopen(argv[optind++], "r");
	}

	if(optind < argc) {
		fprintf(stderr, "Too many arguments: %s\n", optarg);
		exit(EXIT_FAILURE);
	}

	if(!fp) {
		fputs("Must specify a file to read from.\n", stderr);
		exit(EXIT_FAILURE);
	}

	if(desktop_read_file(&entry, fp)) {
		perror("read_desktop_file");
		exit(EXIT_FAILURE);
	}

	fclose(fp);

	if(entry.Path) {
		chdir(entry.Path);
	}

	switch(s) {
		case '\0':
			if(entry.Exec) {
				/* If we ever support %U or %F, this will suck */
				exit(desktop_exec(entry, &system));
			} else {
				fputs("No Exec line found.\n", stderr);
				exit(EXIT_FAILURE);
			}
			break;
		case 'o':
			if(entry.Exec) {
				exit(desktop_exec(entry, &puts));
			} else {
				fputs("No Exec line found.\n", stderr);
				exit(EXIT_FAILURE);
			}
			break;
		case 't':
			if(entry.TryExec) {
				if(file_exists(entry.TryExec) || in_path(entry.TryExec)) {
					exit(EXIT_SUCCESS);
				}
				exit(2);
			} else {
				exit(EXIT_FAILURE);
			}
			break;
		case 'i': {
			char path[PATH_MAX];
			if(entry.Icon) {
				realpath(entry.Icon, path);
				if(file_exists(path)) {
					puts(path);
					break;
				}
				/* TODO Implement http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html */
			} else {
				exit(EXIT_FAILURE);
			}
			break;
		}
		case 'n':
			puts(entry.Name);
			break;
		case 'g':
			puts(entry.GenericName);
			break;
		default:
			/* This should never happen. */
			abort();
	}

	exit(EXIT_SUCCESS);
}
