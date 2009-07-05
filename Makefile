OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_gl
	@echo Building $@

clean:
	@rm *.o -f
	@rm cardy_gl -f

cardy_gl: $(OBJECTS)
	@echo Building $@
	@gcc -g -Os -o $@ $(OBJECTS) -lglut -lGL -lGLU -lm


