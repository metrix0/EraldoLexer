#pragma once

extern void program(void);
extern void expr(void);
extern void smpexpr(void);
extern void ifstmt(void);
extern void whlstmt(void);
extern void repstmt(void);
extern void idlist(void);
extern void block(void);
extern void type(void);
extern void parmlist(void);
extern void stmtlist(void);
extern void stmt(void);

extern int loopcount;

extern FILE *source; // to be defined at main.c
extern FILE *output; // object code
extern int lookahead;
extern char lexeme[];// stores the token string content
// extern void expr(void);
extern void match(int required);
extern int line;
extern int gettoken(FILE *); // defined at lexer.c


