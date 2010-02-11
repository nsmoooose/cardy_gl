OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-background

example-background: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../api -L../../game

clean:
	@rm example-background* *.o *.gcda *.gcno -rf
