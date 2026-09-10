#include <stdio.h>
#include <tokens.h>
#include <ctype.h>

extern int gettoken(FILE *);

FILE *source;

int main(void)
{
	int token;

	while(1) {
		token = gettoken(stdin);
		if (token == EOF) {
			printf("\n");
			return 0;
		}
		if (isascii(token)) {
			printf("%c ", token);
		} else {
			printf("%d ", token);
		}
	}
}


