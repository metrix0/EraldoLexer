// AULA 7: Lexemas e controle de linha.
#pragma once
#include <stdio.h>

#define MAXSTRLEN 256

extern char lexeme[];
extern int line;
extern int tokenline;

// AULA 7: Devolve um token, quebra de linha ou EOF.
int gettoken(FILE *source);

// AULA 7: Avisa o erro e interrompe a expressao atual.
void erro(const char *mensagem);
