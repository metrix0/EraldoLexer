/* vmachine.h - Acumulador e pilha usados na avaliacao das expressoes. */
#pragma once

extern double acc;

/* Guarda o operando esquerdo e o recupera ao concluir o direito. */
void push(void);
double pop(void);

/* Descarta os resultados temporarios entre expressoes, inclusive apos erro. */
void resetvm(void);
