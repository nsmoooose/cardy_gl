include ../config
PLATFORM := $(shell uname)

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

LIBS+=$(shell pkg-config --libs check)

all: cardy_tests

clean:
	@echo Cleaning cardy_tests
	@rm *.o *.gcda *.gcno cardy_tests -f

cardy_tests:  $(OBJECTS) ../api/libcardy-api.a ../game/libcardy-game.a
	@echo Building $@
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)
