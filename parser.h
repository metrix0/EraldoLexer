#pragma once

extern FILE *source; // to be defined at main.c

extern int lookahead;
extern void E(void);
extern void match(int required);
extern int line;
extern int gettoken(FILE *); // defined at lexer.c
