include ../../config
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: example-solved

example-solved: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	@$(RM) example-desktop *.o
