include config
PLATFORM := $(shell uname)

ifdef COVERAGE
	export CFLAGS+=--coverage
endif

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: cardy_gl cardy_server cardy_tests
	@echo Building $@
	@$(MAKE) -C examples $@

clean:
	@echo Cleaning cardy_gl
	@$(RM) *.o cardy_gl* coverage *.gcda *.gcno -rf dist
	@$(MAKE) -C tests $@
	@$(MAKE) -C api $@
	@$(MAKE) -C game $@
	@$(MAKE) -C examples $@

cardy_api:
	@$(MAKE) -C api

cardy_game:
	@$(MAKE) -C game

cardy_server:
	@$(MAKE) -C server

cardy_tests:
	@$(MAKE) -C tests

cardy_gl: cardy_api cardy_game $(OBJECTS)
	@echo Linking $@
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS) -Lapi -Lgame -Lapi/network -Lapi/solitaires -Lgame/backgrounds

ctags:
	@ctags -e --recurse=yes --exclude=analysis/* --exclude=.doxygen/*

format:
	clang-format.exe -i $(shell find -name "*.c" -or -name "*.h")

scan: clean
	scan-build make cardy_gl

deploy:
	@mkdir -p dist/win32
	@cp resources dist/win32/ -r
	@cp themes dist/win32/ -r
	@cp cardy_gl dist/win32/cardy_gl.exe
	./copy_dependencies.sh cardy_gl dist/win32/

install:
	@cp cardy_gl /usr/bin/
	@mkdir /usr/share/cardy_gl/ -p
	@cp -r themes /usr/share/cardy_gl/
	@cp -r resources /usr/share/cardy_gl/

coverage: clean
	@$(MAKE) -C . all COVERAGE=1
	@mkdir coverage
	@echo ===========================================================================
	@-tests/cardy_tests
	@echo ===========================================================================
	@lcov --directory . --capture --output-file coverage/coverage.info
	@echo ===========================================================================
	@genhtml -o coverage/ coverage/coverage.info

.PHONY: all clean cardy_api cardy_game cardy_server cardy_tests ctags format scan deploy coverage
