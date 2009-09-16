PLATFORM := $(shell uname)

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_tests

clean:
	@echo Cleaning cardy_tests
	@rm *.o -f
	@rm *.gcda *.gcno -f
	@rm cardy_tests -f

ifeq ($(PLATFORM), Linux)
cardy_tests:  $(OBJECTS)
	@echo Building $@
	@gcc $(CFLAGS) -o $@ $(OBJECTS) -L../lib $(LIBS) -lcheck
endif

ifeq ($(PLATFORM), MINGW32_NT-5.1)
cardy_tests:
	@echo Tests is not supported on windows.
endif

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
