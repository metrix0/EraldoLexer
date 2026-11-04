// <symtab.c>::
#include <string.h>
#include <lexer.h>
#include <symtab.h>

// typedef struct _symtab_ {
//     char name[MAXSTRLEN+1];
//     int type;
//     char offset[MAXSTRLEN+1];
// } SYMTAB;

SYMTAB symtab[SYMTABSIZE];
int symtab_nextentry = 0;

int symtab_lookup(const char *varname)
{
    int i;
    for (i = 0; i < SYMTABSIZE; i++) {
        if (strcmp(symtab[i].name, varname) == 0) {
            return i;
        }
    }
    return -1; // varname not found
}

int symtab_append(const char *varname)
{
    int found;
    if (symtab_nextentry < SYMTABSIZE) {
        strcpy(symtab[found = symtab_nextentry++].name, varname);
    } else {
        return -1; // symtab overflow
    }
    return found;
}
