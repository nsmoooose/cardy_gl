include ../../config
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-background

example-background: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	@$(RM) example-background* *.o
