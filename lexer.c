#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <tokens.h>
#include <lexer.h>

int line = 1;

char lexeme[MAXSTRLEN+1];

void skipspaces(FILE *tape)
{
	int c;
	while(isspace(c = getc(tape)) ) {
		if (c == '\n') line++;
	}
	ungetc(c, tape);
}

// ASGN = ':='
int isASGN(FILE *tape)
{
	int head = getc(tape);
	if (head == ':') {
		int next = getc(tape);
		if (next == '=') return ASGN;
		ungetc(next, tape);
	}
	ungetc(head, tape);
	return 0;
}

void append(int c, int *i)
{
	if (*i == MAXSTRLEN) {
		fprintf(stderr, "token too long at line %d\n", line);
		exit(-2);
	}
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
			ungetc(head, tape);
			lexeme[i] = 0;
			return OCT;
		}
		ungetc(head, tape);
	}
	ungetc(prefix, tape);
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
				ungetc(head, tape);
				lexeme[i] = 0;
				return HEX;
			}
			ungetc(head, tape);
		}
		ungetc(next, tape);
	}
	ungetc(prefix, tape);
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
		ungetc(head, tape);
		lexeme[i] = 0;
		return ID;
	}
	ungetc(head, tape);
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
			ungetc(head, tape);
		}
		lexeme[i] = 0;
		return DEC;
	}
	ungetc(head, tape);
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

	// if neither pattern above was satisfied return the current ASCII to be read
	token = getc(source);

	return token;
}
