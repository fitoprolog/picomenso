CC=gcc
CFLAGS=-I.
#DEPS = picomenso.h
OBJ = picomenso.o  memory_model.o  testing.o
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tester: $(OBJ)
	$(CC) -o bin/$@ $^ $(CFLAGS) -lm

.PHONY: clean

clean:
	rm -f *.o
	rm -rf bin/* 
