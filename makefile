CC=gcc 
CFLAGS= -pthread -I. -g -w -lm
DEPS =
OBJ = reverse.o
%.o: %.c $(DEPS)
    $(CC) -c -o $@ $< $(CFLAGS)
reverse: $(OBJ) 
    $(CC) -o $@ $^ $(CFLAGS)