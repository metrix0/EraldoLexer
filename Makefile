CFLAGS = -I. -g

EXEC = mybc

$(EXEC): main.o lexer.o parser.o
	$(CC) -o $@ $^

# Limpeza dos arquivos gerados

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)
