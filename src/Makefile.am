bin_PROGRAMS = gnujump

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
gnujump_LDFLAGS = $(all_libraries) 

gnujump_SOURCES = game.c game.h main.c menu.c menu.h records.c records.h \
	SDL_2dgl.c SDL_2dgl.h gnujump.h SDL_rotozoom.c SDL_rotozoom.h setup.c setup.h \
	SFont.c SFont.h sprite.c sprite.h surface.c surface.h tools.c tools.h replay.c \
	replay.h effects.c effects.h gettext.h

datadirdir = $(prefix)/share/$(package)

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
