// AULA 7: Acumulador e pilha de operandos.
#include <lexer.h>
#include <vmachine.h>

#define MAXSTACKSIZE 1024

double acc = 0;
static double stack[MAXSTACKSIZE];
static int sp = -1;

// AULA 7: Guarda o valor enquanto o proximo operando e calculado.
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

// AULA 7: Limpa os temporarios, sem apagar as variaveis.
void resetvm(void)
{
	acc = 0;
	sp = -1;
}
