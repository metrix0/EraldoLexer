#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <keywords.h>
#include <lexer.h>

int line = 1;

char lexeme[MAXSTRLEN+1];

void skipspaces(FILE *tape) // => skipcomments
{
	int c;
	while(isspace(c = getc(tape)) ) {
		if (c == '\n') {
			line++;
		}
	}
	ungetc(c, tape);
}

int isASGN(FILE *tape)
{
	if ( (lexeme[0] = getc(tape)) == ':' ) {
		if ( (lexeme[1] = getc(tape)) == '=' ) {
			lexeme[2] = 0;
			return ASGN;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	return 0;
}

int isRELOP(FILE *tape)
{
	lexeme[0] = getc(tape);
	lexeme[1] = getc(tape);
	lexeme[2] = 0;

	switch(lexeme[0]) {
		case '<':
			switch (lexeme[1]) {
				case '=':
					return LEQ;
				case '>':
					return NEQ;
				default:
					ungetc(lexeme[1], tape);
					lexeme[1] = 0;
					return '<';
			}
		case '>':
			switch (lexeme[1]) {
				case '=':
					return GEQ;
				default:
					ungetc(lexeme[1], tape);
					lexeme[1] = 0;
					return '>';
			}
		case '=':
			ungetc(lexeme[1], tape);
			lexeme[1] = 0;
			return '=';
	}
	ungetc(lexeme[1], tape);
	ungetc(lexeme[0], tape);

	return lexeme[0] = 0;
}

// Guarda o numero sem converter nem arredondar o lexema.
static void numchar(int c, int *i)
{
	if (*i == MAXSTRLEN) {
		fprintf(stderr, "number too long at line %d\n", line);
		exit(EXIT_FAILURE);
	}
	lexeme[(*i)++] = c;
}

// NUM = [0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)? | \.[0-9]+([eE][+-]?[0-9]+)?
int isNUM(FILE *tape)
{
	int c = getc(tape), i = 0, token = UINT;

	if (!isdigit(c) && c != '.') {
		if (c != EOF) ungetc(c, tape);
		lexeme[0] = 0;
		return 0;
	}
	while (isdigit(c)) {
		numchar(c, &i);
		c = getc(tape);
	}
	if (c == '.') {
		int next = getc(tape);
		if ((!i && !isdigit(next)) || next == '.') {
			if (next != EOF) ungetc(next, tape);
			ungetc(c, tape);
			lexeme[i] = 0;
			return i ? UINT : 0;
		}
		token = FLT;
		numchar(c, &i);
		c = next;
		while (isdigit(c)) {
			numchar(c, &i);
			c = getc(tape);
		}
	}
	if (c == 'e' || c == 'E') {
		token = FLT;
		numchar(c, &i);
		c = getc(tape);
		if (c == '+' || c == '-') {
			numchar(c, &i);
			c = getc(tape);
		}
		if (!isdigit(c)) {
			fprintf(stderr, "incomplete exponent at line %d\n", line);
			exit(EXIT_FAILURE);
		}
		do {
			numchar(c, &i);
			c = getc(tape);
		} while (isdigit(c));
	}
	if (c != EOF) ungetc(c, tape);
	lexeme[i] = 0;
	return token;
}

int isID(FILE *tape)
{
	/* 
	   ID = [A-Za-z][A-Za-z0-9]*
	*/

	if ( isalpha((lexeme[0] = getc(tape))) ) {
		int i = 1;
		while( isalnum(lexeme[i] = getc(tape)) || lexeme[i] == '_' ) {
			if (i < MAXSTRLEN) {
				i++;
			}
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		int token;
		if ((token = iskeyword(lexeme))) {
			return token;
		}
		return ID;
	}

	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

int isUINT(FILE *tape)
{
	/*
	  UINT = '0' | [1-9][0-9]*
	*/

	lexeme[0] = getc(tape);

	if ( isdigit(lexeme[0]) ) {
		if (lexeme[0] == '0') {
			lexeme[1] = 0;
			return UINT;
		}
		int i = 1;
		while( isdigit(lexeme[i] = getc(tape)) ) i++;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return UINT;
	}

	ungetc(lexeme[0], tape);

	return 0;
}

int gettoken(FILE *source)
{
	int token;

	skipspaces(source); // -> skipcomments

	if ( (token = isID(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;
	if ( (token = isASGN(source)) ) return token;
	if ( (token = isRELOP(source)) ) return token;

	// if neither pattern above was satisfied return the current ASCII to be read
	token = getc(source);

	return token;
}
