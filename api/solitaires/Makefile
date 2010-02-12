OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-solitaires.a

clean:
	@$(RM) all *.o *.gc*

libcardy-solitaires.a: $(OBJECTS)
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
