#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <tokens.h>
#include <lexer.h>

int line = 1;

char lexeme[MAXSTRLEN+1];

void erro(const char *mensagem)
{
	fprintf(stderr, "Erro na linha %d: %s\n", line, mensagem);
	exit(EXIT_FAILURE);
}

void skipspaces(FILE *tape)
{
	int c;
	while(isspace(c = getc(tape)) ) {
		if (c == '\n') line++;
	}
	if (c != EOF) ungetc(c, tape);
}

// ASGN = ':='
int isASGN(FILE *tape)
{
	int head = getc(tape);
	if (head == ':') {
		int next = getc(tape);
		if (next == '=') return ASGN;
		if (next != EOF) ungetc(next, tape);
	}
	if (head != EOF) ungetc(head, tape);
	return 0;
}

void append(int c, int *i)
{
	// TRATAMENTO DE ERRO: token muito longo
	if (*i == MAXSTRLEN) erro("token muito longo");
	lexeme[(*i)++] = c;
}

// OCT = '0'[0-7]+
int isOCT(FILE *tape)
{
	int prefix = getc(tape);
	if (prefix == '0') {
		int head = getc(tape);
		if ('0' <= head && head <= '7') {
			int i = 0;
			append(prefix, &i);
			do {
				append(head, &i);
				head = getc(tape);
			} while ('0' <= head && head <= '7');
			// TRATAMENTO DE ERRO: digito octal invalido
			if (head == '8' || head == '9') erro("digito invalido em numero octal");
			if (head != EOF) ungetc(head, tape);
			lexeme[i] = 0;
			return OCT;
		}
		if (head != EOF) ungetc(head, tape);
	}
	if (prefix != EOF) ungetc(prefix, tape);
	return 0;
}

// HEX = 0[xX][0-9A-Fa-f]+
int isHEX(FILE *tape)
{
	int prefix = getc(tape);
	if (prefix == '0') {
		int next = getc(tape);
		if (next == 'x' || next == 'X') {
			int head = getc(tape);
			if (isxdigit(head)) {
				int i = 0;
				append(prefix, &i);
				append(next, &i);
				do {
					append(head, &i);
					head = getc(tape);
				} while (isxdigit(head));
				if (head != EOF) ungetc(head, tape);
				lexeme[i] = 0;
				return HEX;
			}
			// TRATAMENTO DE ERRO: hexadecimal incompleto
			erro("numero hexadecimal incompleto");
		}
		if (next != EOF) ungetc(next, tape);
	}
	if (prefix != EOF) ungetc(prefix, tape);
	return 0;
}

// ID = [A-Za-z][A-Za-z0-9]*
int isID(FILE *tape)
{
	int head = getc(tape);
	if (isalpha(head)) {
		int i = 0;
		do {
			append(head, &i);
			head = getc(tape);
		} while (isalnum(head));
		if (head != EOF) ungetc(head, tape);
		lexeme[i] = 0;
		return ID;
	}
	if (head != EOF) ungetc(head, tape);
	return 0;
}

// DEC = '0' | [1-9][0-9]*
int isDEC(FILE *tape)
{
	int head = getc(tape);
	if (isdigit(head)) {
		int i = 0;
		append(head, &i);
		if (head != '0') {
			while (isdigit(head = getc(tape))) append(head, &i);
			if (head != EOF) ungetc(head, tape);
		} else {
			int next = getc(tape);
			// TRATAMENTO DE ERRO: digito invalido apos zero
			if (next == '8' || next == '9') erro("digito invalido em numero octal");
			if (next != EOF) ungetc(next, tape);
		}
		lexeme[i] = 0;
		return DEC;
	}
	if (head != EOF) ungetc(head, tape);
	return 0;
}

int gettoken(FILE *source)
{
	int token;

	skipspaces(source);

	if ( (token = isASGN(source)) ) return token;
	if ( (token = isID(source)) ) return token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isDEC(source)) ) return token;

	token = getc(source);
	if (token == EOF) {
		// TRATAMENTO DE ERRO: falha de leitura
		if (ferror(source)) erro("falha ao ler a entrada");
		return EOF;
	}
	if (token == '+' || token == '-' || token == '*' || token == '/' ||
	    token == '(' || token == ')') return token;

	char mensagem[80];
	if (token == ':') {
		// TRATAMENTO DE ERRO: atribuicao incompleta
		erro("esperado '=' depois de ':'");
	} else if (isprint(token)) {
		snprintf(mensagem, sizeof mensagem, "caractere invalido '%c'", token);
	} else {
		snprintf(mensagem, sizeof mensagem, "caractere invalido (0x%02X)", token);
	}
	// TRATAMENTO DE ERRO: caractere nao reconhecido
	erro(mensagem);
	return EOF;
}
