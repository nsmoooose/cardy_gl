PLATFORM := $(shell uname)

export CFLAGS=-g -Os -Wall -pedantic $(shell pkg-config --cflags glib-2.0 gdk-pixbuf-2.0 cairo librsvg-2.0)

ifdef COVERAGE
	export CFLAGS+=--coverage
endif

ifeq ($(PLATFORM), Linux)
LIBS=-lglut -lGL -lGLU -lm -lcardy $(shell pkg-config --libs glib-2.0 gdk-pixbuf-2.0 cairo librsvg-2.0)
endif

ifeq ($(PLATFORM), MINGW32_NT-5.1)
LIBS=-lcardy -lglu32 -lglut32 -lm -lopengl32 $(shell pkg-config --libs glib-2.0 gdk-pixbuf-2.0 cairo librsvg-2.0)
endif

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_gl
	@echo Building $@

clean:
	@echo Cleaning cardy_gl
	@rm *.o -f
	@rm cardy_gl -f
	@rm coverage -rf
	@rm *.gcda *.gcno -f
	@make -C tests $@
	@make -C lib $@

cardy_lib:
	@make -C lib

cardy_tests:
	@make -C tests

cardy_gl: cardy_lib cardy_tests $(OBJECTS)
	@echo Building $@
	@echo gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -Llib
	@gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -Llib
	@make -C tests

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<

coverage:
	@mkdir coverage
	@echo ===========================================================================
	@tests/cardy_tests
	@echo ===========================================================================
	@lcov --directory . --capture --output-file coverage/coverage.info
	@echo ===========================================================================
	@genhtml -o coverage/ coverage/coverage.info
