include ../config
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_server

clean:
	@rm *.o *.gcda *.gcno -f

cardy_server: $(OBJECTS)
	@echo Linking $@
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)
