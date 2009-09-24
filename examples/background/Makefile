OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-background

example-background: $(OBJECTS) ../../lib/libcardy.a
	@gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../lib

clean:
	@rm example-background* *.o *.gcda *.gcno -rf

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
