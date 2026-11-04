// <parser.c>::

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <symtab.h>
#include <keywords.h>
#include <parser.h>

// lookahead is the compiler's eye which is traditionally defined here
int             lookahead;

void program(void) // checked
{
	match(PROGRAM);
	match(ID);
	match('(');
	idlist(); // ok
	match(')');
	match(';');
	block();
	match('.');
}

void idlist(void) // checked
{
	// for each string naming variable or function, it must be appended, avoiding duplicates, in the symbol table
	_IDHEAD:
	// call symtab_append(const char *varname) before match
	match(ID);
	if (lookahead == ',') {
		match(',');
		goto _IDHEAD;
	}
}

void block(void)
{
	if (lookahead == VAR) {
		match(VAR);
		_idlist_head:
			idlist();
			match(':');
			type();
			match(';');
			if (lookahead == ID) {
				goto _idlist_head;
			}
	}
	while (lookahead == PROCEDURE || lookahead == FUNCTION) {
		int funcflg = (lookahead == FUNCTION);
		match(lookahead);
		// append subporgram name to the symbol table regarding the specificity of procedure or function
		match(ID);
		parmlist();
		if (funcflg) {
			match(':');
			type();
		}
		match(';');
		block();
		match(';');
	}
	match(BEGIN);
	stmtlist();
	match(END);
}

void parmlist(void)
{
	if (lookahead == '(') {
		match('(');
		_again:
			if (lookahead == VAR) {
				match(VAR);
			}
			idlist(); // all the symbols declared in this context must have different offsets than declared in block()
			match(':');
			type();
			if (lookahead == ';') {
				match(';');
				goto _again;
			}
		match(')');
	}
}


void stmtlist(void)
{
	_stmt_head:
		stmt();
		if (lookahead == ';') {
			match(';');
			goto _stmt_head;
		}
}

void arglist(void)
{
	if (lookahead == '(') {
		match('(');
		_expr_head:
		expr();
		if (lookahead == ',') {
			match(',');
			goto _expr_head;
		}
		match(')');
	}
}

int loopcount = 1;

void stmt(void)
{
	int lbl1, lbl2;
	switch(lookahead) {
		case ID:
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				expr();
			} else {
				arglist();
			}
			break;
		case BEGIN:
			match(BEGIN); stmtlist(); match(END);
			break;
		case IF:
			match(IF); // if head
			expr(); // <expr.as>
			/*[[1]]*/fprintf(output, "\tjz .L%d\n", lbl1 = lbl2 = loopcount++);/*[[1]]*/
			match(THEN);
			stmt(); // <stmt.as>
			if (lookahead == ELSE) {
				/*[[2]]*/
				// jmp .endiflabel
				fprintf(output, "\tjmp .L%d\n", lbl2 = loopcount++);
				// .elselabel:
				fprintf(output, ".L%d:\n", lbl1);
				/*[[2]]*/
				match(ELSE);
				stmt(); // <stmt.as>
			}
			// .endiflabel:
			/*[[3]]*/fprintf(output, ".L%d:\n", lbl2);/*[[3]]*/
			break;
		case REPEAT:
			// .replbl:
			/*[[1]]*/fprintf(output, ".L%d:\n", lbl1 = loopcount++);/*[[1]]*/
			match(REPEAT);
			stmtlist();
			match(UNTIL);
			expr();
			// jz .replbl
			/*[[2]]*/fprintf(output, "\tjz .L%d\n", lbl1);/*[[2]]*/
			break;
		case WHILE:
			// .while:
			/*[[1]]*/fprintf(output, ".L%d:\n", lbl1 = loopcount++);/*[[1]]*/
			match(WHILE);
			expr();
			// jz .endwhile
			/*[[2]]*/fprintf(output, "\tjz .L%d\n", lbl2 = loopcount++);/*[[2]]*/
			match(DO);
			stmt(); // <stmt.as>
			// jmp .while
			/*[[3]]*/fprintf(output, "\tjm .L%d\n", lbl1);/*[[3]]*/
			// .endwhile:
			/*[[4]]*/fprintf(output, ".L%d:\n", lbl2);/*[[4]]*/
			break;
		default:
			; // empty statement
	}
}

void type(void)
{
	switch(lookahead) {
		case INTEGER:
			match(INTEGER); break;
		case REAL:
			match(REAL); break;
		case DOUBLE:
			match(DOUBLE); break;
		default:
			match(BOOLEAN);
	}
}

int isrelop(void)
{
	switch (lookahead) {
		case '<':
		case LEQ:
		case NEQ:
		case '>':
		case GEQ:
		case '=':
			return lookahead;
	}
	return 0;
}
void expr(void) // expr types must be numerical or boolean
{
	smpexpr();
	if (isrelop()) {
		match(lookahead);
		smpexpr();
	}
}

int isotimes(void)
{
	if(lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND)
		return lookahead;
	return 0;
}
void smpexpr(void)
{
	// char name[MAXSTRLEN+1];

	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
	}

  _T:

  _F:

	switch (lookahead) {
	case ID:
		match(ID); // ID is associated to a R-value or L-value variable
		if (lookahead == ASGN) {
			// ID := E -> L-value
			match(ASGN);
			expr();
		} else {
			// R-value
		}
		break;
	case FLT:
		match(FLT);
		break;
	case UINT:
		match(UINT);
		break;
	default:
		match('(');
		expr();
		match(')');
	}

	if (isotimes()) {
		match(lookahead);
		goto _F;
	}

	// term ends here

	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
		goto _T;
	}

}

void match(int required)
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

int isFIRST(void)
{
	switch (lookahead) {
		case '+':
		case '-':
		case ID:
		case FLT:
		case UINT:
		case '(':
			return lookahead;
	}
	return 0;
}
