// <parser.c>::

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>

// lookahead is the compiler's eye which is traditionally defined here
int             lookahead;
extern char lexeme[];// stores the token string content

// virtual machine definition
// accumulator:
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
	// TRATAMENTO DE ERRO: pilha vazia
	if (sp < 0) erro("pilha vazia");
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
	// check prefixing signal
	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			sigflag = 1; // signal seen
		}
		match(lookahead);
	}

  _T:
	// T();
  _F:
	// F();
	switch (lookahead) {
	case ID:
		// fprintf(output, " %s", lexeme);
		strcpy(name, lexeme);
		match(ID); // ID is associated to a R-value or L-value variable
		if (lookahead == ASGN) {
			// ID := E -> L-value
			match(ASGN);
			E();
			store(name, acc);
		} else {
			// R-value
			acc = recall(name);
		}
		break;
	case DEC:
		// TRATAMENTO DE ERRO: decimal fora do limite
		errno = 0;
		acc = strtod(lexeme, NULL);
		if (errno == ERANGE) erro("numero fora do limite");
		match(DEC);
		break;
	case OCT:
		// TRATAMENTO DE ERRO: octal fora do limite
		errno = 0;
		acc = strtol(lexeme, NULL, 8);
		if (errno == ERANGE) erro("numero fora do limite");
		match(OCT);
		break;
	case HEX:
		// TRATAMENTO DE ERRO: hexadecimal fora do limite
		errno = 0;
		acc = strtol(lexeme, NULL, 16);
		if (errno == ERANGE) erro("numero fora do limite");
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

	// term ends
	if (sigflag) {
		acc = -acc;
		sigflag = 0; // turn off sigflag
	}

	if (oplusflg) {
		// fprintf(output, " %c", oplusflg);
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

/*
void
T(void)
{
  _F:
	// F();
	switch (lookahead) {
	case ID:
		match(ID);
		break;
	case DEC:
		match(DEC);
		break;
	case OCT:
		match(OCT);
		break;
	case HEX:
		match(HEX);
		break;
	default:
		match('(');
		E();
		match(')');
	}

	if (lookahead == '*' || lookahead == '/') {
		match(lookahead);
		goto _F;
	}
}

void
F(void)
{
	switch (lookahead) {
	case ID:
		match(ID);
		break;
	case DEC:
		match(DEC);
		break;
	case OCT:
		match(OCT);
		break;
	case HEX:
		match(HEX);
		break;
	default:
		match('(');
		E();
		match(')');
	}
}
*/

static void mostra_token(int token)
{
	switch (token) {
	case EOF: fprintf(stderr, "fim da entrada"); break;
	case ID: fprintf(stderr, "identificador"); break;
	case DEC: fprintf(stderr, "numero decimal"); break;
	case OCT: fprintf(stderr, "numero octal"); break;
	case HEX: fprintf(stderr, "numero hexadecimal"); break;
	case ASGN: fprintf(stderr, "':='"); break;
	default:
		if (token >= 0 && token <= 255 && isprint(token))
			fprintf(stderr, "'%c'", token);
		else
			fprintf(stderr, "token %d", token);
	}
}

void match(int required)
{
	// TRATAMENTO DE ERRO: token inesperado
	if (lookahead != required) {
		fprintf(stderr, "Erro de sintaxe na linha %d: esperado ", line);
		mostra_token(required);
		fprintf(stderr, ", encontrado ");
		mostra_token(lookahead);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
	lookahead = gettoken(source);
}
