OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-game.a

clean:
	@echo Cleaning libcardy-game
	@rm *.o libcardy-game.a *.gcda *.gcno -f
	@$(MAKE) -C backgrounds clean

libcardy-game.a: $(OBJECTS)
	@$(MAKE) -C backgrounds
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
