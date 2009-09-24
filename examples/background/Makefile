OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-background

example-background: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../api -L../../game

clean:
	@rm example-background* *.o *.gcda *.gcno -rf

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
