OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libcardy-api.a

clean:
	@echo Cleaning libcardy-api
	@$(RM) libcardy-api.a *.o *.gc*
	@$(MAKE) -C network clean
	@$(MAKE) -C solitaires clean

libcardy-api.a: $(OBJECTS)
	@$(MAKE) -C network
	@$(MAKE) -C solitaires
	@echo Creating library $@
	$(AR) rcs $@ $(OBJECTS)
