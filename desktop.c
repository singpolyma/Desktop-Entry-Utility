#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(__WIN32__)
   #include "getopt.h"
#else
   #include <unistd.h>
#endif

#include "libdesktop.h"

void help(char *name) {
	puts("Extract data from a .destkop (Desktop Entry) file.");
	puts("With no switches, execut the processed Exec line.");
	printf("Usage: %s [OPTION] [PATH]\n", name);
	puts("   PATH    Path to .desktop file");
	puts("   -h      help (this screen)");
	puts("   -o      output processed Exec line");
	puts("   -t      exit status 0 on TryExec exists, 2 on doesn't exist, 1 on no such key");
	puts("   -i      output absolute path to icon");
	puts("   -n      output name");
	puts("   -g      output generic name");
}

FILE *xfopen(char *pth, char *mode) {
	FILE *fp;
	if(!(fp = fopen(pth, mode))) {
		fprintf(stderr, "Failed to open file '%s'", pth);
		exit(EXIT_FAILURE);
	}
	return fp;
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

	switch(s) {
		case '\0':
			/* If we ever support %U or %F, this will suck */
			exit(desktop_exec(entry, &system));
			break;
		case 'o':
			exit(desktop_exec(entry, &puts));
			break;
		case 't':
			/* TODO */
			break;
		case 'i':
			/* TODO */
			break;
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
