/* parser.c - Reconhece expressoes e executa as acoes de interpretacao. */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <symtab.h>
#include <vmachine.h>

/* Token corrente, consultado antes de cada decisao sintatica. */
int lookahead;

/* Ponto de retorno instalado pelo main para abandonar somente a linha atual. */
jmp_buf recovery;

/*
 * Mantem os niveis de E, T e F no mesmo procedimento: _T inicia um termo
 * e _F inicia um fator. Parenteses e atribuicoes recorrem a E.
 * As marcas numeradas delimitam as acoes semanticas; legenda no README.
 */
void E(void)
{
	char name[MAXSTRLEN+1];
	/* O sinal prefixado vale para o primeiro termo, como na versao anterior. */
	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			/*6*/ sigflag = 1; /*6*/
		}
		match(lookahead);
	}

  _T:
  _F:
	switch (lookahead) {
	case ID:
		/*7*/ strcpy(name, lexeme); /*7*/
		match(ID); /* O token seguinte distingue leitura de atribuicao. */
		if (lookahead == ASGN) {
			/* A recursao preserva a associatividade direita de ID := E. */
			match(ASGN);
			E();
			/*9*/ store(name, acc); /*9*/
		} else {
			/* Uso do valor de uma variavel ja definida. */
			/*8*/ acc = recall(name); /*8*/
		}
		break;
	case DEC:
		/*5*/ acc = strtod(lexeme, NULL); /*5*/
		match(DEC);
		break;
	case OCT:
		/*5*/ acc = strtol(lexeme, NULL, 8); /*5*/
		match(OCT);
		break;
	case HEX:
		/*5*/ acc = strtol(lexeme, NULL, 16); /*5*/
		match(HEX);
		break;
	case '(':
		match('(');
		E();
		match(')');
		break;
	default:
		expected("numero, variavel ou '('");
	}

	/*4*/
	if (otimesflg) {
		if (otimesflg == '*') {
			acc = pop() * acc;
		} else {
			/* O operando direito esta em acc; o esquerdo esta na pilha. */
			if (acc == 0) erro("divisao por zero");
			acc = pop() / acc;
		}
		otimesflg = 0;
	}
	/*4*/

	if (lookahead == '*' || lookahead == '/') {
		/*3*/
		push();
		otimesflg = lookahead;
		/*3*/
		match(lookahead);
		goto _F;
	}

	/* O termo termina antes de resolver a soma ou subtracao pendente. */
	/*6*/
	if (sigflag) {
		acc = -acc;
		sigflag = 0;
	}

	/*6*/

	/*2*/
	if (oplusflg) {
		if (oplusflg == '+') {
			acc = acc + pop();
		} else {
			acc = pop() - acc;
		}
		oplusflg = 0;
	}
	/*2*/

	if (lookahead == '+' || lookahead == '-') {
		/*1*/
		push();
		oplusflg = lookahead;
		/*1*/
		match(lookahead);
		goto _T;
	}

}

/* Nomes legiveis dos tokens, sem expor os numeros internos do enum. */
static void tokenname(char *text, size_t size)
{
	switch (lookahead) {
	case EOF: snprintf(text, size, "fim da entrada"); break;
	case '\n': snprintf(text, size, "fim da linha"); break;
	case ID: snprintf(text, size, "variavel '%s'", lexeme); break;
	case DEC: case OCT: case HEX:
		snprintf(text, size, "numero '%s'", lexeme); break;
	case ASGN: snprintf(text, size, "':='"); break;
	default:
		if (lookahead >= 0 && lookahead <= 255 && isprint(lookahead))
			snprintf(text, size, "'%c'", lookahead);
		else
			snprintf(text, size, "caractere de codigo %d", lookahead);
	}
}

/* Informa tanto o que falta quanto o token que impediu a analise. */
void expected(const char *description)
{
	char found[MAXSTRLEN + 20];
	char message[MAXSTRLEN + 120];
	tokenname(found, sizeof found);
	snprintf(message, sizeof message, "esperado %s; encontrado %s",
			description, found);
	erro(message);
}

/* Zera o token antes da leitura: um erro lexico nao deixa lookahead antigo. */
void nexttoken(void)
{
	lookahead = 0;
	lookahead = gettoken(source);
}

void match(int required)
{
	if (lookahead != required) {
		char description[32];
		if (required == EOF) strcpy(description, "fim da entrada");
		else if (required == '\n') strcpy(description, "fim da linha");
		else snprintf(description, sizeof description, "'%c'", required);
		expected(description);
	}
	nexttoken();
}

/*
 * Recuperacao em modo panico: descarta o restante da linha com erro.
 * Se o lexer ja leu a quebra de linha, a proxima expressao nao e consumida.
 */
void recover(void)
{
	int c;
	if (lookahead == '\n' || lookahead == EOF) return;
	while ((c = getc(source)) != EOF && c != '\n');
	if (c == '\n') line++;
}
