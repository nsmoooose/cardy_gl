OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_gl
	@echo Building $@

clean:
	@echo Cleaning cardy_gl
	@rm *.o -f
	@rm cardy_gl -f
	@make -C tests $@
	@make -C lib $@

cardy_lib:
	@make -C lib

cardy_tests:
	@make -C tests

cardy_gl: cardy_lib cardy_tests $(OBJECTS)
	@echo Building $@
	@gcc -g -Os -o $@ $(OBJECTS) -lglut -lGL -lGLU -lm -lcardy -Llib
	@make -C tests
