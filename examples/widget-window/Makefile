OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: widget-window

widget-window: $(OBJECTS) ../../api/libcardy-api.a ../../game/libcardy-game.a
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -L../../api -L../../game

clean:
	@rm widget-window *.o *.gcda *.gcno -rf
