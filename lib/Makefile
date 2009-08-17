OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy.a

clean:
	@echo Cleaning libcardy

	@rm *.o -f
	@rm libcardy.a -f

libcardy.a: $(OBJECTS)
	@echo Creating library $@
	@ar rcs $@ $(OBJECTS)

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
