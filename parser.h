#pragma once

extern FILE *source; // definido em main.c

extern int lookahead;
extern void E(void);
extern void match(int required);
extern int line;
extern int gettoken(FILE *); // definido em lexer.c
