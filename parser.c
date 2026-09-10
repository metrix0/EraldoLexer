//<parser.c>::

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <parser.h>

// lookahead is the compiler's eye which is traditionally defined here
int lookahead;

/*
  LL(1) recursive parser requires an LL(1) grammar.

  LL(1) for expressions

  E -> T R
  T -> F Q
  R -> '+' T R | '-' T R | <empty>
  Q -> '*' F Q | '/' F Q | <empty>
  F -> ID | DEC | '(' E ')'
*/

//  E -> ['+''-'] T R
void E(void)
{	// check prefixing signal
	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
	}
	T(); R();
}

//  T -> F Q
void T(void)
{
	F(); Q();
}

//  R -> ['+''-'] T R | <empty>
void R(void)
{
	if (lookahead == '+' || lookahead == '-') { match(lookahead); T(); R(); }
	else { ; }
}

//  Q -> ['*''/'] F Q | <empty>
void Q(void)
{
	if (lookahead == '*' || lookahead == '/') { match(lookahead); F(); Q(); }
	else { ; }
}

//  F -> ID | DEC | '(' E ')'
void F(void)
{
	switch(lookahead) {
	case ID:
		match(ID); break;
	case DEC:
		match(DEC); break;
	default:
		match('('); E(); match(')');
	}
}

void match(int required)
{
	if (lookahead == required) {
		lookahead = gettoken(source);
	} else {
		if (isprint(lookahead)) {
			fprintf(stderr, "token mismatch: %c while expected %c\nexiting with err\n", 
				lookahead, required);
		} else {
		        if (lookahead == -1) {
				fprintf(stderr, "premature EOF seen\n");
			} else {
				fprintf(stderr, 
					"token mismatch: %d while expected %d\nexiting with err\n", 
				lookahead, required);
			}
		}
		exit(-2);
	}
}
