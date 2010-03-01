OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-backgrounds.a

clean:
	@$(RM) all *.o *.gc*

libcardy-backgrounds.a: $(OBJECTS)
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
