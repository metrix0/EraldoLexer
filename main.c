#include <stdio.h>
#include <tokens.h>
#include <main.h>
#include <parser.h>

FILE *source;

int main(void)
{
	source = stdin;
	lookahead = gettoken(source);
	E();
	match(EOF);

	printf("%lg\n", acc);
	return 0;
}
