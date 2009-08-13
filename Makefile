PLATFORM := $(shell uname)

ifeq ($(PLATFORM), Linux)
LIBS=-lglut -lGL -lGLU -lm -lcardy
endif

ifeq ($(PLATFORM), MINGW32_NT-5.1)
LIBS=-lglut32 -lGLU32 -lm -lcardy -lopengl32
endif

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
	@echo gcc -Wall -pedantic -g -Os -o $@ $(OBJECTS) $(LIBS) -Llib
	@gcc -Wall -pedantic -g -Os -o $@ $(OBJECTS) $(LIBS) -Llib
	@make -C tests

%.o: %.c
	@echo $<
	@gcc -c -g -Os -Wall -pedantic $<
