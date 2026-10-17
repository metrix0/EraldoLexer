/* symtab.h - Armazenamento e consulta das variaveis do MyBC. */
#pragma once

/* Cria a variavel se necessario; uma nova atribuicao substitui seu valor. */
void store(char *name, double value);

/* Consulta o valor, informando erro quando o nome ainda nao foi definido. */
double recall(char *name);
