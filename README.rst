Building
========

Linux
-----

TODO

Windows
-------

Dependencies:

* Install MSYS2 and use MINGW64 terminal.
* Install the following dependencies::

    pacman -S mingw64/mingw-w64-x86_64-librsvg
    pacman -S mingw64/mingw-w64-x86_64-pkgconf
    pacman -S mingw64/mingw-w64-x86_64-freeglut

Building::

    ./configure
    make
