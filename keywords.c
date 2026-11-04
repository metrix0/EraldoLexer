// <keywords.c>::
#include <string.h>
#include <keywords.h>

char *keywordlist[] =
{
    "begin",
    "if",
    "then",
    "else",
    "while",
    "do",
    "repeat",
    "until",
    "not",
    "or",
    "div",
    "mod",
    "and",
    "true",
    "false",
    "program",
    "procedure",
    "function",
    "var",
    "integer",
    "real",
    "double",
    "boolean",
    "end",
};


long unsigned iskeyword(const char *identifier)
{
    for (int i = BEGIN; i <= END; i++) {
        if (strcmp(keywordlist[i - BEGIN], identifier) == 0) {
            return i;
        }
    }
    return 0;
}

// demorou!
