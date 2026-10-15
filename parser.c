#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>

// Proximo token a ser analisado
int lookahead;

// Acumulador usado nas operacoes
double acc = 0;

#define MAXSTACKSIZE 1024
double stack[MAXSTACKSIZE];
int sp = -1;
void push(void)
{
	// TRATAMENTO DE ERRO: pilha cheia
	if (sp == MAXSTACKSIZE - 1) erro("pilha cheia");
	sp++;
	stack[sp] = acc;
}
double pop(void)
{
	double aux = stack[sp];
	sp--;
	return aux;
}

#define SYMTABSIZE 1024
double vmem[SYMTABSIZE];
char symtab[SYMTABSIZE][MAXSTRLEN+1];
int symcount = 0;

int lookup(char *name)
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
		// TRATAMENTO DE ERRO: tabela de simbolos cheia
		if (symcount == SYMTABSIZE) erro("tabela de simbolos cheia");
		i = symcount++;
		strcpy(symtab[i], name);
	}
	vmem[i] = value;
}

double recall(char *name)
{
	int i = lookup(name);
	// TRATAMENTO DE ERRO: variavel nao definida
	if (i == -1) {
		char mensagem[MAXSTRLEN + 40];
		snprintf(mensagem, sizeof mensagem, "variavel '%s' nao definida", name);
		erro(mensagem);
	}
	return vmem[i];
}

void E(void)
{
	char name[MAXSTRLEN+1];
	// Verifica se a expressao comeca com sinal negativo
	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			sigflag = 1;
		}
		match(lookahead);
	}

  _T:
  _F:
	switch (lookahead) {
	case ID:
		strcpy(name, lexeme);
		match(ID);
		if (lookahead == ASGN) {
			// Atribuicao: guarda o resultado na variavel
			match(ASGN);
			E();
			store(name, acc);
		} else {
			// Consulta o valor da variavel
			acc = recall(name);
		}
		break;
	case DEC:
		acc = strtod(lexeme, NULL);
		match(DEC);
		break;
	case OCT:
		acc = strtol(lexeme, NULL, 8);
		match(OCT);
		break;
	case HEX:
		acc = strtol(lexeme, NULL, 16);
		match(HEX);
		break;
	case '(':
		match('(');
		E();
		match(')');
		break;
	default:
		// TRATAMENTO DE ERRO: expressao incompleta
		erro("expressao esperada");
	}

	if (otimesflg) {
		if (otimesflg == '*') {
			acc = pop() * acc;
		} else {
			// TRATAMENTO DE ERRO: divisao por zero
			if (acc == 0) erro("divisao por zero");
			acc = pop() / acc;
		}
		otimesflg = 0;
	}

	if (lookahead == '*' || lookahead == '/') {
		push();
		otimesflg = lookahead;
		match(lookahead);
		goto _F;
	}

	// Fim do termo
	if (sigflag) {
		acc = -acc;
		sigflag = 0;
	}

	if (oplusflg) {
		if (oplusflg == '+') {
			acc = acc + pop();
		} else {
			acc = pop() - acc;
		}
		oplusflg = 0;
	}

	if (lookahead == '+' || lookahead == '-') {
		push();
		oplusflg = lookahead;
		match(lookahead);
		goto _T;
	}

}

void match(int required)
{
	// TRATAMENTO DE ERRO: token inesperado
	if (lookahead != required) {
		fprintf(stderr, "Erro de sintaxe na linha %d\n", line);
		exit(EXIT_FAILURE);
	}
	lookahead = gettoken(source);
}
