OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-game.a

clean:
	@echo Cleaning libcardy-game

	@rm *.o -f
	@rm libcardy-game.a -f
	@rm *.gcda *.gcno -f

libcardy-game.a: $(OBJECTS)
	@echo Creating library $@
	@ar rcs $@ $(OBJECTS)
