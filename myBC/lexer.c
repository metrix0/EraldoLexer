// AULA 7: Le os tokens da entrada.
#include <stdio.h>
#include <ctype.h>
#include <parser.h>
#include <tokens.h>
#include <lexer.h>

int line = 1;
int tokenline = 1;

char lexeme[MAXSTRLEN+1];

// AULA 7: Volta ao main depois de avisar o erro.
void erro(const char *mensagem)
{
	fprintf(stderr, "Erro na linha %d: %s\n", tokenline, mensagem);
	longjmp(recovery, 1);
}

// AULA 7: Deixa a quebra de linha para o parser.
void skipspaces(FILE *tape)
{
	int c;
	while ((c = getc(tape)) != '\n' && isspace(c));
	if (c != EOF) ungetc(c, tape);
}

// AULA 7: Reconhece :=.
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

// AULA 7: Confere o limite do lexema.
void append(int c, int *i)
{
	if (*i == MAXSTRLEN) erro("token muito longo");
	lexeme[(*i)++] = c;
}

// AULA 7: Octal: zero seguido de digitos de 0 a 7.
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

// AULA 7: Hexadecimal: prefixo 0x ou 0X.
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

			// AULA 7: Devolve a quebra de linha para nao pular a seguinte.
			if (head != EOF) ungetc(head, tape);
			erro("numero hexadecimal incompleto");
		}
		if (next != EOF) ungetc(next, tape);
	}
	if (prefix != EOF) ungetc(prefix, tape);
	return 0;
}

// AULA 7: Nome comeca por letra e pode ter numeros.
int isID(FILE *tape)
{
	int head = getc(tape);

	if (isalpha(head)) {
		lexeme[0] = head;
		int i = 1;
		while (isalnum(head = getc(tape))) {
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

// AULA 7: Numero inteiro decimal.
int isDEC(FILE *tape)
{
	int head = getc(tape);

	if (isdigit(head)) {
		lexeme[0] = head;
		if (head == '0') {
			lexeme[1] = 0;
			return DEC;
		}

		int i = 1;
		while (isdigit(head = getc(tape))) {
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

// AULA 7: Tenta cada tipo e devolve o token encontrado.
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

	token = getc(source);
	if (token == '\n') line++;
	return token;
}
