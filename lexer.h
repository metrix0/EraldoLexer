/* lexer.h - Tokens, lexemas e posicao da leitura na entrada. */
#pragma once
#include <stdio.h>

#define MAXSTRLEN 256

extern char lexeme[];
extern int line;       /* Linha da proxima leitura do arquivo. */
extern int tokenline;  /* Linha de origem do token corrente. */

/* Retorna o token, '\n' como separador de expressoes, ou EOF. */
int gettoken(FILE *source);

/* Reporta o erro e retorna ao ponto de recuperacao instalado pelo main. */
void erro(const char *mensagem);
