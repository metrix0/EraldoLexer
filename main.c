/* main.c - Le expressoes da entrada padrao, uma por linha, ate EOF. */
#include <stdio.h>
#include <main.h>
#include <lexer.h>
#include <parser.h>
#include <vmachine.h>

FILE *source;

int main(void)
{
	int status = 0;
	source = stdin;

	for (;;) {
		/* Um erro abandona a chamada de E, mas nao encerra o interpretador. */
		if (setjmp(recovery)) {
			status = 1;
			recover();
			resetvm();
			continue;
		}
		resetvm();
		nexttoken();
		if (lookahead == EOF) break;
		if (lookahead == '\n') continue;

		E();
		if (lookahead != '\n' && lookahead != EOF)
			expected("fim da linha");

		printf("%lg\n", acc);
		if (lookahead == EOF) break;
	}
	return status;
}
