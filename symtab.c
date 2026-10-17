/* symtab.c - Cada nome ocupa a mesma posicao que seu valor na memoria. */
#include <stdio.h>
#include <string.h>
#include <lexer.h>
#include <symtab.h>

#define SYMTABSIZE 1024
static double vmem[SYMTABSIZE];
static char symtab[SYMTABSIZE][MAXSTRLEN+1];
static int symcount = 0;

/* A busca linear basta para a tabela pequena usada pelo interpretador. */
static int lookup(char *name)
{
	int i;
	for (i = 0; i < symcount; i++) {
		if (strcmp(symtab[i], name) == 0) return i;
	}
	return -1;
}

void store(char *name, double value)
{
	int i = lookup(name);
	if (i == -1) {
		if (symcount == SYMTABSIZE) erro("tabela de simbolos cheia");
		i = symcount++;
		strcpy(symtab[i], name);
	}
	vmem[i] = value;
}

double recall(char *name)
{
	int i = lookup(name);
	if (i == -1) {
		char mensagem[MAXSTRLEN + 40];
		snprintf(mensagem, sizeof mensagem, "variavel '%s' nao definida", name);
		erro(mensagem);
	}
	return vmem[i];
}
