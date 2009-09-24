OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-api.a

clean:
	@echo Cleaning libcardy-api

	@rm *.o -f
	@rm libcardy-api.a -f
	@rm *.gcda *.gcno -f

libcardy-api.a: $(OBJECTS)
	@echo Creating library $@
	@ar rcs $@ $(OBJECTS)

%.o: %.c
	@echo $<
	@gcc -c $(CFLAGS) $<
