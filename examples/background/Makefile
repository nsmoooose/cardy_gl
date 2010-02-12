OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-background

example-background: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../api -L../../game -L../../api/network -L../../api/solitaires

clean:
	@rm example-background* *.o *.gcda *.gcno -rf
