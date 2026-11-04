CFLAGS = -I$(include) -g

EXEC = mypas

include = ./include

$(EXEC): main.o lexer.o parser.o symtab.o keywords.o
	$(CC) -o $@ $^

# depolution targets:

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)
