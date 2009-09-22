OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: widget-window

widget-window: $(OBJECTS) ../../lib/libcardy.a
	@gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../lib

clean:
	@rm widget-window *.o *.gcda *.gcno -rf

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
