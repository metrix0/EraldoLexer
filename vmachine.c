/* vmachine.c - Maquina virtual simples, com um acumulador e uma pilha. */
#include <lexer.h>
#include <vmachine.h>

#define MAXSTACKSIZE 1024

double acc = 0;
static double stack[MAXSTACKSIZE];
static int sp = -1;

/* A pilha preserva operandos enquanto o parser calcula subexpressoes. */
void push(void)
{
	if (sp == MAXSTACKSIZE - 1) erro("pilha cheia");
	sp++;
	stack[sp] = acc;
}

double pop(void)
{
	if (sp < 0) erro("pilha vazia");
	double aux = stack[sp];
	sp--;
	return aux;
}

void resetvm(void)
{
	acc = 0;
	sp = -1;
}
