#include <stdio.h>
#include "loops.h"

// AULA 10: Contador compartilhado pelos lacos.
int nextlabel = 1;

// AULA 10: WHILE expr DO stmt.
void whilestmt(void)
{
	int start, end;

	match(WHILE);
	// AULA 10: Acao 1 - marca o inicio do teste.
	start = nextlabel++;
	end = nextlabel++;
	printf(".L%d:\n", start);

	expr();
	match(DO);
	// AULA 10: Acao 2 - sai quando a condicao e falsa.
	printf("\ttestq %%rax, %%rax\n");
	printf("\tjz .L%d\n", end);

	stmt();
	// AULA 10: Acao 3 - volta ao teste e marca a saida.
	printf("\tjmp .L%d\n", start);
	printf(".L%d:\n", end);
}

// AULA 10: REPEAT stmt { ; stmt } UNTIL expr.
void repeatstmt(void)
{
	int start;

	match(REPEAT);
	// AULA 10: Acao 1 - marca o inicio do corpo.
	start = nextlabel++;
	printf(".L%d:\n", start);

	stmt();
	while (lookahead == ';') {
		match(';');
		stmt();
	}
	match(UNTIL);
	expr();
	// AULA 10: Acao 2 - repete enquanto a condicao e falsa.
	printf("\ttestq %%rax, %%rax\n");
	printf("\tjz .L%d\n", start);
}
