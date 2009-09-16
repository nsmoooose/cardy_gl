OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-desktop

example-desktop: $(OBJECTS)
	@gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../lib

clean:
	@rm example-desktop *.o *.gcda *.gcno -rf

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
