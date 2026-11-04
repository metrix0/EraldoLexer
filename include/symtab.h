// <symtab.h>::

#pragma once

#include <lexer.h>

#define SYMTABSIZE 1024

typedef struct _symtab_ {
    char name[MAXSTRLEN+1];
    int type;
    char offset[MAXSTRLEN+1];
} SYMTAB;

extern SYMTAB symtab[];
extern int symtab_nextentry;
extern int symtab_lookup(const char *varname);
extern int symtab_append(const char *varname);
