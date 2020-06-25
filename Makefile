CFLAGS  = -I .
DEPS = hellomake.h
OBJS = binaryfield.o binaryfield_tests.o

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

binaryfield: $(OBJS)
	gcc -o  $@ $^ $(CFLAGS)
