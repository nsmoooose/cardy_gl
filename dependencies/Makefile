DEP_RSVG_SOURCE=http://ftp.acc.umu.se/pub/GNOME/sources/librsvg/2.26/librsvg-2.26.0.tar.bz2
DEP_RSVG_FILE=librsvg-2.26.0.tar.bz2
DEP_RSVG_FOLDER=librsvg-2.26.0

export CC=/usr/bin/i686-pc-mingw32-gcc
export NM=/usr/bin/i686-pc-mingw32-nm
export AR=/usr/bin/i686-pc-mingw32-ar
export RANLIB=/usr/bin/i686-pc-mingw32-ranlib
export PKG_CONFIG=/usr/bin/i686-pc-mingw32-pkg-config

all: $(DEP_RSVG_FOLDER)/.libs/librsvg-2.a

$(DEP_RSVG_FOLDER)/.libs/librsvg-2.a: $(DEP_RSVG_FOLDER)
	cd $(DEP_RSVG_FOLDER) && ./configure && $(MAKE)

$(DEP_RSVG_FOLDER): $(DEP_RSVG_FILE)
	tar xf $(DEP_RSVG_FILE)

$(DEP_RSVG_FILE):
	wget $(DEP_RSVG_SOURCE)

clean:
	$(RM) -r $(DEP_RSVG_FOLDER)
