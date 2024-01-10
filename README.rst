Building
========

Linux
-----

Dependencies:

* Install the following dependencies::

    dnf install librsvg2-devel freeglut-devel

* Install additional packages if you are planning on more in depth
  development::

    dnf install check-devel

Building::

    ./configure linux
    make cardy_gl

Windows
-------

Dependencies:

* Install MSYS2 and use MINGW64 terminal.
* Install the following dependencies::

    pacman -S mingw64/mingw-w64-x86_64-librsvg
    pacman -S mingw64/mingw-w64-x86_64-pkgconf
    pacman -S mingw64/mingw-w64-x86_64-freeglut

* Install additional packages if you are planning on more in depth
  development::

    pacman -S mingw64/mingw-w64-x86_64-jq
    pacman -S mingw64/mingw-w64-x86_64-check

Building::

    ./configure windows
    make cardy_gl


Running
=======

Normally::

    ./cardy_gl

Debug mode (shows unfinished solitaires as well)::

    ./cardy_gl -d

Architecture
============

Libraries
---------

* api
* api/network
* api/solitaires
* game
* game/backgrounds

Rendering
---------

At the root you have a rendering context with a single render_object in it.
Each render_object can have children with new render_object. This is then
forming a tree structure of objects::

    render_context
     |- render_object
         |- children

Render objects is currently only big things like widgets (buttons), background.
Individual cards are NOT a rendering object. Solitaire is currently handle by
a single rendering object that does everything to handle the drawing of cards.

How rendered objects are places now::

    background (triangles)
     |- placeholder ()
     |   |- solitaire (the card game you have selected)
     |   |- solitaire_solved (solved)
     |- desktop (rotating svg)
         |- topmenu (buttons on the)
         |- mainmenu (select solitaire to play)
