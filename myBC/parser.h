// AULA 7: Funcoes do parser.
#pragma once
#include <stdio.h>
#include <setjmp.h>

extern FILE *source;
extern int lookahead;
extern jmp_buf recovery;

void E(void);
void match(int required);
void nexttoken(void);
void expected(const char *description);
void recover(void);
