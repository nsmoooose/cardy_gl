OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy.a

clean:
	@echo Cleaning libcardy

	@rm *.o -f
	@rm libcardy.a -f

libcardy.a: $(OBJECTS)
	@echo Building $@
	@ar rcs $@ $(OBJECTS)
	# @gcc --shared -g -Os -o $@ $(OBJECTS) -lglut -lGL -lGLU -lm -lcheck
