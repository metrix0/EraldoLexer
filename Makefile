CFLAGS = -I. -g

EXEC = mybc

$(EXEC): main.o lexer.o parser.o symtab.o vmachine.o
	$(CC) -o $@ $^

# Recompila os objetos que dependem de um cabecalho alterado.
main.o: main.h lexer.h parser.h vmachine.h
lexer.o: lexer.h parser.h tokens.h
parser.o: parser.h lexer.h tokens.h symtab.h vmachine.h
symtab.o: symtab.h lexer.h
vmachine.o: vmachine.h lexer.h

.PHONY: clean mostlyclean

# Limpeza para a entrega dos fontes:

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)
