#pragma once

enum { WHILE = 1024, DO, REPEAT, UNTIL };

extern int lookahead;
extern int nextlabel;

void match(int token);

// AULA 10: Expr deixa a condicao em %rax; stmt traduz um comando.
void expr(void);
void stmt(void);

void whilestmt(void);
void repeatstmt(void);
