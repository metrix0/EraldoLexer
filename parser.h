/* parser.h - Interface da analise e da recuperacao por linha. */
#pragma once
#include <stdio.h>
#include <setjmp.h>

extern FILE *source; /* Entrada definida em main.c. */
extern int lookahead;
extern jmp_buf recovery;

void E(void);
void match(int required);
void nexttoken(void);
void expected(const char *description);
void recover(void);
