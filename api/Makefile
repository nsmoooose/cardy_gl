OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-api.a

clean:
	@echo Cleaning libcardy-api
	@rm libcardy-api.a *.o *.gc* solitaires/*.o solitaires/*.gc* network/*.o network/*.gc* -f

libcardy-api.a: $(OBJECTS)
	@$(MAKE) -C network
	@$(MAKE) -C solitaires
	@echo Creating library $@
	@$(AR) rcs $@ $(OBJECTS)
