// <keywords.h>::
#pragma once

extern char *keywordlist[];
extern long unsigned iskeyword(const char *);

enum _KEYWORD_TOKENS
{
    BEGIN = 2048,
    IF,
    THEN,
    ELSE,
    WHILE,
    DO,
    REPEAT,
    UNTIL,
    NOT,
    OR,
    DIV,
    MOD,
    AND,
    TRUE,
    FALSE,
    PROGRAM,
    PROCEDURE,
    FUNCTION,
    VAR,
    INTEGER,
    REAL,
    DOUBLE,
    BOOLEAN,
    END,
};
