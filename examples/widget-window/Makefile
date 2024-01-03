include ../../config
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: widget-window

widget-window: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	@$(RM) widget-window* *.o
