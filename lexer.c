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
		lexeme[0] = head;
		int i = 1;
		while (isalnum(head = getc(tape))) {
			// TRATAMENTO DE ERRO: token muito longo
			if (i == MAXSTRLEN) erro("token muito longo");
			lexeme[i++] = head;
		}
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
		lexeme[0] = head;
		if (head == '0') {
			lexeme[1] = 0;
			return DEC;
		}

		int i = 1;
		while (isdigit(head = getc(tape))) {
			// TRATAMENTO DE ERRO: token muito longo
			if (i == MAXSTRLEN) erro("token muito longo");
			lexeme[i++] = head;
		}
		if (head != EOF) ungetc(head, tape);
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
	return token;
}
