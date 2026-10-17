CFLAGS = -I. -g

EXEC = mybc

$(EXEC): main.o lexer.o parser.o symtab.o vmachine.o
	$(CC) -o $@ $^

# AULA 7: Dependencias dos cabecalhos.
main.o: main.h lexer.h parser.h vmachine.h
lexer.o: lexer.h parser.h tokens.h
parser.o: parser.h lexer.h tokens.h symtab.h vmachine.h
symtab.o: symtab.h lexer.h
vmachine.o: vmachine.h lexer.h

.PHONY: clean mostlyclean

# AULA 7: Limpa os arquivos gerados.

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)
