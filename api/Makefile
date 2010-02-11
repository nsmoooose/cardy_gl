OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.c,%.o,$(wildcard solitaires/*.c)) $(patsubst %.c,%.o,$(wildcard network/*.c))

all: libcardy-api.a

clean:
	@echo Cleaning libcardy-api
	@rm libcardy-api.a *.o *.gc* solitaires/*.o solitaires/*.gc* network/*.o network/*.gc* -f

libcardy-api.a: $(OBJECTS)
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
