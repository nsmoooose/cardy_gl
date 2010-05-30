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
	@rm *.o cardy_gl* coverage *.gcda *.gcno -rf
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

style:
	@find -name "*.h" -or -name "*.c"|xargs astyle --suffix=none

deploy:
	@mkdir dist/win32 -p
	@cp resources dist/win32/ -r
	@cp themes dist/win32/ -r
	@cp cardy_gl.exe dist/win32/
	@cp /c/devlibs/bin/libcairo-2.dll dist/win32/
	@cp /c/devlibs/bin/libfontconfig-1.dll dist/win32/
	@cp /c/devlibs/bin/freetype6.dll dist/win32/
	@cp /c/devlibs/bin/libexpat.dll dist/win32/
	@cp /c/devlibs/bin/libpng12-0.dll dist/win32/
	@cp /c/devlibs/bin/zlib1.dll dist/win32/
	@cp /mingw/bin/glee.dll dist/win32/
	@cp /mingw/bin/glut32.dll dist/win32/
	@cp /mingw/bin/librsvg-2-2.dll dist/win32/
	@cp /c/devlibs/bin/libcroco-0.6-3.dll dist/win32/
	@cp /c/devlibs/bin/libglib-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libxml2.dll dist/win32/
	@cp /c/devlibs/bin/iconv.dll dist/win32/
	@cp /c/devlibs/bin/libgdk_pixbuf-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libgio-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libgmodule-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libgobject-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libpango-1.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libpangocairo-1.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libpangowin32-1.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libpangoft2-1.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libgmodule-2.0-0.dll dist/win32/
	@cp /c/devlibs/bin/libgmodule-2.0-0.dll dist/win32/

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

cppcheck:
	@mkdir -p analysis-cppcheck
	@$(RM) analysis-cppcheck/*
	cppcheck . --enable=all --xml 2> analysis-cppcheck/errors.xml
	cppcheck-htmlreport --file analysis-cppcheck/errors.xml --report-dir=analysis-cppcheck --source-dir=. --title cardy_gl
