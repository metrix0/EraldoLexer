// AULA 7: Le uma expressao por linha.
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
		// AULA 7: Depois do erro, continua na proxima linha.
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
