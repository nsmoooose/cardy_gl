PLATFORM := $(shell uname)

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_tests

clean:
	@echo Cleaning cardy_tests
	@rm *.o *.gcda *.gcno cardy_tests -f

ifeq ($(PLATFORM), Linux)
cardy_tests:  $(OBJECTS) ../api/libcardy-api.a ../game/libcardy-game.a
	@echo Building $@
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) -L../api -L../game $(LIBS) -lcheck
endif

ifeq ($(PLATFORM), MINGW32_NT-5.1)
cardy_tests:
	@echo Tests is not supported on windows.
endif
