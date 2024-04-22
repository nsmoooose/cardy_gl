include config
PLATFORM := $(shell uname)

ifdef COVERAGE
	export CFLAGS+=--coverage
endif

OBJECTS_API = $(patsubst %.c,%.o,$(wildcard api/*.c))
OBJECTS_API_NETWORK = $(patsubst %.c,%.o,$(wildcard api/network/*.c))
OBJECTS_API_SOLITAIRES = $(patsubst %.c,%.o,$(wildcard api/solitaires/*.c))
OBJECTS_CLIENT = $(patsubst %.c,%.o,$(wildcard client/*.c))
OBJECTS_CLIENT_BACKGROUNDS = $(patsubst %.c,%.o,$(wildcard client/backgrounds/*.c))
OBJECTS_SERVER = $(patsubst %.c,%.o,$(wildcard server/*.c))
OBJECTS_TESTS = $(patsubst %.c,%.o,$(wildcard tests/*.c))
OBJECTS_EXAMPLES_BACKGROUND = $(patsubst %.c,%.o,$(wildcard examples/background/*.c))
OBJECTS_EXAMPLES_SOLVED = $(patsubst %.c,%.o,$(wildcard examples/solved/*.c))
OBJECTS_EXAMPLES_WIDGET_WINDOW = $(patsubst %.c,%.o,$(wildcard examples/widget-window/*.c))

ifeq ($(MSYSTEM), MINGW64)
OBJECTS_CLIENT+=client/resource.res.o
endif

all: cardy_gl cardy_server cardy_tests

clean:
	@echo Cleaning cardy_gl cardy_server cardy_tests
	$(RM) -rf coverage dist cardy_gl cardy_server cardy_tests example_background example_solved example_widget-window
	$(RM) tests/*.o tests/cardy_tests tests/*.gcda tests/*.gcno
	$(RM) api/*.o api/*.gcda api/*.gcno
	$(RM) api/network/*.o api/network/*.gcda api/network/*.gcno
	$(RM) api/solitaires/*.o api/solitaires/*.gcda api/solitaires/*.gcno
	$(RM) client/*.o client/*.gcda client/*.gcno
	$(RM) client/backgrounds/*.o client/backgrounds/*.gcda client/backgrounds/*.gcno

cardy_server: $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

cardy_tests: $(OBJECTS_API) $(OBJECTS_API_NETWORK) $(OBJECTS_API_SOLITAIRES) $(OBJECTS_TESTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LIBS_TESTS)

examples: example_background example_solved example_widget

example_background: $(OBJECTS_API) $(OBJECTS_CLIENT_BACKGROUNDS) $(OBJECTS_EXAMPLES_BACKGROUND)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

example_solved: $(OBJECTS_API) $(OBJECTS_CLIENT_BACKGROUNDS) $(OBJECTS_EXAMPLES_SOLVED)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

example_widget-window: $(OBJECTS_API) $(OBJECTS_CLIENT_BACKGROUNDS) $(OBJECTS_EXAMPLES_WIDGET_WINDOW)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

ifeq ($(MSYSTEM), MINGW64)
client/resource.res.o: client/resource.rc
	windres $^ $@
endif

cardy_gl: $(OBJECTS_API) $(OBJECTS_API_SOLITAIRES) $(OBJECTS_CLIENT) $(OBJECTS_CLIENT) $(OBJECTS_CLIENT_BACKGROUNDS)
	@echo Linking $@
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

ctags:
	@ctags -e --recurse=yes --exclude=analysis/* --exclude=.doxygen/*

format:
	clang-format -i $(shell find -name "*.c" -or -name "*.h")

scan: clean
	scan-build make cardy_gl

deploy: cardy_gl
	@mkdir -p dist/win32
	@cp resources dist/win32/ -r
	@cp cardy_gl dist/win32/cardy_gl.exe
	@strip dist/win32/cardy_gl.exe
	./copy_dependencies.sh cardy_gl dist/win32/

install: cardy_gl
	@cp cardy_gl /usr/bin/
	@mkdir /usr/share/cardy_gl/ -p
	@cp -r resources /usr/share/cardy_gl/

coverage: clean
	@$(MAKE) -C . cardy_tests COVERAGE=1
	@mkdir coverage
	@echo ===========================================================================
	@-./cardy_tests
	@echo ===========================================================================
	@lcov --directory . --capture --output-file coverage/coverage.info
	@echo ===========================================================================
	@genhtml -o coverage/ coverage/coverage.info

.PHONY: all clean ctags deploy examples format scan deploy coverage install
