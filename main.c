#include <stdio.h>
#include <string.h>

int term(const char *port);

void usage(const char *tool)
{
	char *name = NULL;
	name = strrchr(tool, '/');
	printf("usage : %s [PORT]\n",(name ? name + 1: tool));
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
		return 0;
	}

	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
	{
		usage(argv[0]);
		return 0;
	}

	term(argv[0]);

	return 0;
}