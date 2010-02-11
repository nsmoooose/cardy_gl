OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-game.a

clean:
	@echo Cleaning libcardy-game
	@rm *.o libcardy-game.a *.gcda *.gcno -f

libcardy-game.a: $(OBJECTS)
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
