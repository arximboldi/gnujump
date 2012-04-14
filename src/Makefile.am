bin_PROGRAMS = gnujump

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
gnujump_LDFLAGS = $(all_libraries) 

gnujump_SOURCES = \
	game.h \
	menu.h \
	records.h \
	SDL_2dgl.h \
	gnujump.h\
	SDL_rotozoom.h \
	setup.h \
	SFont.h \
	sprite.h \
	surface.h \
	effects.h \
	gettext.h \
	replay.h \
	tools.h \
	game.c\
	game-input.c\
	game-output.c\
	game-logic.c\
	game-tools.c\
	game-timer.c\
	main.c\
	menu.c\
	menu-system.c\
	records.c\
	SDL_2dgl.c\
	SDL_rotozoom.c \
	setup.c \
	SFont.c \
	sprite.c \
	surface.c \
	tools.c \
	replay.c \
	effects-trail.c \
	effects-blur.c

datadirdir = $(prefix)/share/$(package)

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
