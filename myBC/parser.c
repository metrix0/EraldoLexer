// AULA 7: Analise das expressoes.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <symtab.h>
#include <vmachine.h>

int lookahead;

// AULA 7: Retorno usado quando ocorre um erro.
jmp_buf recovery;

// AULA 7: _T trata os termos e _F trata os fatores.
void E(void)
{
	char name[MAXSTRLEN+1];

	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			// AULA 7: Acao 6 - guarda o sinal.
			sigflag = 1;
		}
		match(lookahead);
	}

  _T:
  _F:
	switch (lookahead) {
	case ID:
		// AULA 7: Acao 7 - salva o nome antes do match.
		strcpy(name, lexeme);
		match(ID);
		if (lookahead == ASGN) {
			match(ASGN);
			E();
			// AULA 7: Acao 9 - salva o resultado na variavel.
			store(name, acc);
		} else {
			// AULA 7: Acao 8 - le o valor da variavel.
			acc = recall(name);
		}
		break;
	case DEC:
		// AULA 7: Acao 5 - converte o decimal.
		acc = strtod(lexeme, NULL);
		match(DEC);
		break;
	case OCT:
		// AULA 7: Acao 5 - converte o octal.
		acc = strtol(lexeme, NULL, 8);
		match(OCT);
		break;
	case HEX:
		// AULA 7: Acao 5 - converte o hexadecimal.
		acc = strtol(lexeme, NULL, 16);
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

	// AULA 7: Acao 4 - calcula a multiplicacao ou divisao.
	if (otimesflg) {
		if (otimesflg == '*') {
			acc = pop() * acc;
		} else {
			if (acc == 0) erro("divisao por zero");
			acc = pop() / acc;
		}
		otimesflg = 0;
	}

	if (lookahead == '*' || lookahead == '/') {
		// AULA 7: Acao 3 - guarda o operando e o operador.
		push();
		otimesflg = lookahead;

		match(lookahead);
		goto _F;
	}

	// AULA 7: Acao 6 - aplica o sinal.
	if (sigflag) {
		acc = -acc;
		sigflag = 0;
	}

	// AULA 7: Acao 2 - calcula a soma ou subtracao.
	if (oplusflg) {
		if (oplusflg == '+') {
			acc = acc + pop();
		} else {
			acc = pop() - acc;
		}
		oplusflg = 0;
	}

	if (lookahead == '+' || lookahead == '-') {
		// AULA 7: Acao 1 - guarda o operando e o operador.
		push();
		oplusflg = lookahead;

		match(lookahead);
		goto _T;
	}

}

// AULA 7: Usa nomes legiveis nas mensagens.
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

// AULA 7: Mostra o que faltou e o que foi encontrado.
void expected(const char *description)
{
	char found[MAXSTRLEN + 20];
	char message[MAXSTRLEN + 120];
	tokenname(found, sizeof found);
	snprintf(message, sizeof message, "esperado %s; encontrado %s",
			description, found);
	erro(message);
}

// AULA 7: Evita reaproveitar o token anterior se a leitura falhar.
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

// AULA 7: Pula o resto da linha sem consumir a proxima.
void recover(void)
{
	int c;
	if (lookahead == '\n' || lookahead == EOF) return;
	while ((c = getc(source)) != EOF && c != '\n');
	if (c == '\n') line++;
}
