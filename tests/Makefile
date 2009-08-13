PLATFORM := $(shell uname)

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_tests

clean:
	@echo Cleaning cardy_tests
	@rm *.o -f
	@rm cardy_tests -f

ifeq ($(PLATFORM), Linux)
cardy_tests:  $(OBJECTS)
	@echo Building $@
	@gcc -g -Os -o $@ $(OBJECTS) -lglut -lGL -lGLU -lm -lcheck -lcardy -L../lib
endif

ifeq ($(PLATFORM), MINGW32_NT-5.1)
cardy_tests:
	@echo Tests is not supported on windows.
endif

%.o: %.c
	@echo $<
	@gcc -c -g -Os -Wall -pedantic $<
