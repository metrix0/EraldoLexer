#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <main.h>
#include <parser.h>

FILE *source;

int main(int argc, char **argv)
{
	// TRATAMENTO DE ERRO: quantidade de argumentos
	if (argc > 2) {
		fprintf(stderr, "Uso: %s [arquivo]\n", argv[0]);
		return EXIT_FAILURE;
	}

	source = stdin;
	if (argc == 2) {
		source = fopen(argv[1], "r");
		// TRATAMENTO DE ERRO: falha ao abrir arquivo
		if (source == NULL) {
			perror(argv[1]);
			return EXIT_FAILURE;
		}
	}

	lookahead = gettoken(source);
	E();
	match(EOF);

	printf("%lg\n", acc);

	if (source != stdin) fclose(source);
	return EXIT_SUCCESS;
}
