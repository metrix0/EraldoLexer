#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>

int Line = 1;

void skipcomments(FILE *tape)
{
	int head;

	while ((head = getc(tape)) != EOF) {

		if (head == '\n')
			Line++;

		/*
		 * \( and \) are part of the comment text,
		 * so an escaped ')' must not end the comment.
		 */
		if (head == '\\') {
			head = getc(tape);

			if (head == EOF)
				return;

			if (head == '\n')
				Line++;

			continue;
		}

		/* ')' without '\' ends the comment */
		if (head == ')')
			return;
	}
}


void skipspaces(FILE *tape)
{
	int c;

	while (isspace(c = getc(tape))) {
		if (c == '\n')
			Line++;
	}

	ungetc(c, tape);
}


int isID(FILE *tape)
{
	/* 
	   ID = [A-Za-z][A-Za-z0-9]*
	*/

	int head = getc(tape);

	if ( isalpha(head) ) {
		while( isalnum(head = getc(tape)) );
		ungetc(head, tape);
		return ID;
	}

	ungetc(head, tape);
	return 0;
}


int isDEC(FILE *tape)
{
	/*
	  DEC = '0' | [1-9][0-9]*
	*/

	int head = getc(tape);

	if ( isdigit(head) ) {
		if (head == '0') {
			return DEC;
		}

		while( isdigit(head = getc(tape)) );
		ungetc(head, tape);
		return DEC;
	}

	ungetc(head, tape);
	return 0;
}


int gettoken(FILE *source)
{
	int token;

	/*
	 * Ignore spaces and comments before looking
	 * for the next actual token.
	 */
	while (1) {
		skipspaces(source);

		token = getc(source);

		if (token == EOF)
			return EOF;

		if (token == '(') {
			skipcomments(source);
			continue;
		}

		ungetc(token, source);
		break;
	}

	if ( (token = isID(source)) ) return token;
	if ( (token = isDEC(source)) ) return token;

	// if neither pattern above was satisfied return the current ASCII to be read
	token = getc(source);

	return token;
}