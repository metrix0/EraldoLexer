// <parser.c>::

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
	if (sp == MAXSTACKSIZE - 1) {
		fprintf(stderr, "stack full at line %d\n", line);
		exit(-2);
	}
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
		if (symcount == SYMTABSIZE) {
			fprintf(stderr, "symbol table full\n");
			exit(-2);
		}
		i = symcount++;
		strcpy(symtab[i], name);
	}
	vmem[i] = value;
}

double recall(char *name)
{
	int i = lookup(name);
	if (i == -1) {
		fprintf(stderr, "undefined variable: %s at line %d\n", name, line);
		exit(-2);
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
	default:
		match('(');
		E();
		match(')');
	}

	if (otimesflg) {
		if (otimesflg == '*') {
			acc = pop() * acc;
		} else {
			if (acc == 0) {
				fprintf(stderr, "division by zero at line %d\n", line);
				exit(-2);
			}
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

void
match(int required)
{
	if (lookahead == required) {
		lookahead = gettoken(source);
	} else {
		if (isprint(lookahead)) {
			fprintf(stderr,
					"token mismatch: %c while expected %c\nexiting with err\n",
					lookahead, required);
		} else {
			if (lookahead == EOF) {
				fprintf(stderr, "premature EOF seen at line %d\n", line);
			} else {
				if (isprint(lookahead)) {
					fprintf(stderr,
							"token mismatch: %c while expected %c at line %d\nexiting with err\n",
							lookahead, required, line);
				} else {
					fprintf(stderr,
							"token mismatch: %d while expected %d at line %d\nexiting with err\n",
							lookahead, required, line);
				}
			}
		}
		exit(-2);
	}
}
