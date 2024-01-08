import os
import subprocess

packages = [
    "glib-2.0",
    "gdk-pixbuf-2.0",
    "cairo",
    "librsvg-2.0",
    "libxml-2.0",
    "pangocairo gio-2.0",
]

RULES = """rule cc
  command = gcc $cflags -c $in -o $out

"""

def cflags_get(packages):
    cflags = "-g -Os -Wall -pedantic -Werror -I."
    if os.name == 'nt':
        cflags += " -mwindows"

    p = subprocess.run(["pkg-config", "--cflags"] + packages, check=True, capture_output=True, text=True)
    return "%s %s" % (cflags, p.stdout)

def libs_get(packages):
    libs = "-lcardy-game -lcardy-backgrounds -lcardy-api -lcardy-network -lcardy-solitaires -lglu32 -lm -lopengl32 -lws2_32 -Lapi -L/game -Lapi/network -Lapi/solitaires -Lgame/backgrounds"

    p = subprocess.run(["pkg-config", "--libs"] + packages, check=True, capture_output=True, text=True)
    return "%s %s" % (libs, p.stdout)


with open("build.ninja", "wt") as f:
    f.write("cflags = %s" % cflags_get(packages))
    f.write("libs = %s" % libs_get(packages))
    f.write("\n")
    f.write(RULES)
    f.write("cardy_gl: cc program.c\n")
