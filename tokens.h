/* tokens.h - Tokens compostos; os simbolos simples usam seu codigo ASCII. */
#pragma once

enum {
	ID = 1024,
	DEC,
	OCT,
	HEX,
	ASGN, // := right-associative assignment operator
};
