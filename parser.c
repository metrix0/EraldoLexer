#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>

int lookahead;
double acc = 0;

#define SYMTABSIZE 1024
double vmem[SYMTABSIZE];
char symtab[SYMTABSIZE][MAXSTRLEN+1];
int symcount = 0;

int lookup(char *name)
{
    int i;
    for (i = 0; i < symcount; i++) {
        if (strcmp(symtab[i], name) == 0) return i;
    }
    return -1;
}

void store(char *name, double value)
{
    int i = lookup(name);
    if (i == -1) {
        // TRATAMENTO DE ERRO: tabela de simbolos cheia
        if (symcount == SYMTABSIZE) erro("tabela de simbolos cheia");
        i = symcount++;
        strcpy(symtab[i], name);
    }
    vmem[i] = value;
}

double recall(char *name)
{
    int i = lookup(name);
    // TRATAMENTO DE ERRO: variavel nao definida
    if (i == -1) {
        char mensagem[MAXSTRLEN + 40];
        snprintf(mensagem, sizeof mensagem, "variavel '%s' nao definida", name);
        erro(mensagem);
    }
    return vmem[i];
}

void E(void)
{
    int negativo = 0;
    if (lookahead == '+' || lookahead == '-') {
        negativo = (lookahead == '-');
        match(lookahead);
    }
    T();
    if (negativo) acc = -acc;
    R();
}

void T(void)
{
    F();
    Q();
}

void R(void)
{
    while (lookahead == '+' || lookahead == '-') {
        int op = lookahead;
        double esquerdo = acc;
        match(op);
        T();
        if (op == '+') acc = esquerdo + acc;
        else acc = esquerdo - acc;
    }
}

void Q(void)
{
    while (lookahead == '*' || lookahead == '/') {
        int op = lookahead;
        double esquerdo = acc;
        match(op);
        F();
        if (op == '*') acc = esquerdo * acc;
        else {
            // TRATAMENTO DE ERRO: divisao por zero
            if (acc == 0) erro("divisao por zero");
            acc = esquerdo / acc;
        }
    }
}

void F(void)
{
    char name[MAXSTRLEN+1];
    switch (lookahead) {
    case ID:
        strcpy(name, lexeme);
        match(ID);
        if (lookahead == ASGN) {
            match(ASGN);
            E();
            store(name, acc);
        } else {
            acc = recall(name);
        }
        break;
    case DEC:
        acc = strtod(lexeme, NULL);
        match(DEC);
        break;
    case OCT:
        acc = strtol(lexeme, NULL, 8);
        match(OCT);
        break;
    case HEX:
        acc = strtol(lexeme, NULL, 16);
        match(HEX);
        break;
    case '(':
        match('(');
        E();
        match(')');
        break;
    default:
        // TRATAMENTO DE ERRO: expressao incompleta
        erro("expressao esperada");
    }
}

void match(int required)
{
    // TRATAMENTO DE ERRO: token inesperado
    if (lookahead != required) {
        fprintf(stderr, "Erro de sintaxe na linha %d\n", line);
        exit(EXIT_FAILURE);
    }
    lookahead = gettoken(source);
}
