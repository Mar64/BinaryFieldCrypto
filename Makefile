CFLAGS  = -I .
DEPS = binaryfield.h binaryfield_tests.h binaryfield_benchmark.h
OBJS = binaryfield.o binaryfield_tests.o binaryfield_benchmark.o main.o

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

binaryfield: $(OBJS)
	gcc -o  $@ $^ $(CFLAGS)
