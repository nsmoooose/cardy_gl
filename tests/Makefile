OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_tests

clean:
	@echo Cleaning cardy_tests
	@rm *.o -f
	@rm cardy_tests -f

cardy_tests:  $(OBJECTS)
	@echo Building $@
	@gcc -g -Os -o $@ $(OBJECTS) -lglut -lGL -lGLU -lm -lcheck
