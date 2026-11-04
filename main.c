#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <main.h>

FILE *source, *output;

int main(int argc, char const *argv[])
{
	if (argc > 1) {
		source = fopen(argv[1], "r");
		if (!source) {
			perror(argv[1]);
			return 1;
		}
	} else {
		source = stdin;
	}

	const char *outputfname = argc > 2 ? argv[2] : "a.out";
	output = fopen(outputfname, "w");
	if (!output) {
		perror(outputfname);
		if (source != stdin) fclose(source);
		return 1;
	}

	lookahead = gettoken(source);

	program();

	int status = ferror(source) ? 1 : 0;
	if (source != stdin && fclose(source) == EOF) status = 1;
	if (fclose(output) == EOF) {
		perror(outputfname);
		status = 1;
	}
	return status;
}
