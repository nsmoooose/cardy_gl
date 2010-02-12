OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-network.a

clean:
	@$(RM) all *.o *.gc*

libcardy-network.a: $(OBJECTS)
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
