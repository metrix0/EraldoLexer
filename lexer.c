/*
 * lexer.c - Reconhece identificadores, inteiros e atribuicao.
 * Os reconhecedores devolvem os caracteres que nao pertencem ao token.
 */
#include <stdio.h>
#include <ctype.h>
#include <parser.h>
#include <tokens.h>
#include <lexer.h>

int line = 1;
int tokenline = 1;

char lexeme[MAXSTRLEN+1];

/* Um unico diagnostico por linha; a continuacao e feita pelo main. */
void erro(const char *mensagem)
{
	fprintf(stderr, "Erro na linha %d: %s\n", tokenline, mensagem);
	longjmp(recovery, 1);
}

/* Ignora espacos horizontais; a quebra de linha delimita uma expressao. */
void skipspaces(FILE *tape)
{
	int c;
	while ((c = getc(tape)) != '\n' && isspace(c));
	if (c != EOF) ungetc(c, tape);
}

/* ASGN = ':='; dois caracteres formam um unico token. */
int isASGN(FILE *tape)
{
	int head = getc(tape);
	if (head == ':') {
		int next = getc(tape);
		if (next == '=') return ASGN;
		if (next != EOF) ungetc(next, tape);
	}
	if (head != EOF) ungetc(head, tape);
	return 0;
}

/* Reserva o ultimo byte para o terminador do lexema. */
void append(int c, int *i)
{
	/* Impede escrita alem do limite do lexema. */
	if (*i == MAXSTRLEN) erro("token muito longo");
	lexeme[(*i)++] = c;
}

// OCT = '0'[0-7]+
int isOCT(FILE *tape)
{
	int prefix = getc(tape);
	if (prefix == '0') {
		int head = getc(tape);
		if ('0' <= head && head <= '7') {
			int i = 0;
			append(prefix, &i);
			do {
				append(head, &i);
				head = getc(tape);
			} while ('0' <= head && head <= '7');
			if (head != EOF) ungetc(head, tape);
			lexeme[i] = 0;
			return OCT;
		}
		if (head != EOF) ungetc(head, tape);
	}
	if (prefix != EOF) ungetc(prefix, tape);
	return 0;
}

// HEX = 0[xX][0-9A-Fa-f]+
int isHEX(FILE *tape)
{
	int prefix = getc(tape);
	if (prefix == '0') {
		int next = getc(tape);
		if (next == 'x' || next == 'X') {
			int head = getc(tape);
			if (isxdigit(head)) {
				int i = 0;
				append(prefix, &i);
				append(next, &i);
				do {
					append(head, &i);
					head = getc(tape);
				} while (isxdigit(head));
				if (head != EOF) ungetc(head, tape);
				lexeme[i] = 0;
				return HEX;
			}
			/* Preserva o separador para a recuperacao nao pular a proxima linha. */
			if (head != EOF) ungetc(head, tape);
			erro("numero hexadecimal incompleto");
		}
		if (next != EOF) ungetc(next, tape);
	}
	if (prefix != EOF) ungetc(prefix, tape);
	return 0;
}

int isID(FILE *tape)
{
	/* 
	   ID = [A-Za-z][A-Za-z0-9]*
	*/
	int head = getc(tape);

	if (isalpha(head)) {
		lexeme[0] = head;
		int i = 1;
		while (isalnum(head = getc(tape))) {
			/* Impede escrita alem do limite do lexema. */
			if (i == MAXSTRLEN) erro("token muito longo");
			lexeme[i++] = head;
		}
		if (head != EOF) ungetc(head, tape);
		lexeme[i] = 0;
		return ID;
	}

	if (head != EOF) ungetc(head, tape);
	return 0;
}

int isDEC(FILE *tape)
{
	/*
	  DEC = '0' | [1-9][0-9]*
	*/
	int head = getc(tape);

	if (isdigit(head)) {
		lexeme[0] = head;
		if (head == '0') {
			lexeme[1] = 0;
			return DEC;
		}

		int i = 1;
		while (isdigit(head = getc(tape))) {
			/* Impede escrita alem do limite do lexema. */
			if (i == MAXSTRLEN) erro("token muito longo");
			lexeme[i++] = head;
		}
		if (head != EOF) ungetc(head, tape);
		lexeme[i] = 0;
		return DEC;
	}

	if (head != EOF) ungetc(head, tape);
	return 0;
}

/* A ordem dos reconhecedores distingue octais e hexadecimais de zero. */
int gettoken(FILE *source)
{
	int token;

	skipspaces(source);
	tokenline = line;

	if ( (token = isASGN(source)) ) return token;
	if ( (token = isID(source)) ) return token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isDEC(source)) ) return token;

	/* Operadores e parenteses usam seu proprio codigo de caractere. */
	token = getc(source);
	if (token == '\n') line++;
	return token;
}
