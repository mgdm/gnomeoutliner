bin_PROGRAMS = gnome-outliner

gnome_outliner_SOURCES = \
    outliner-marshal.h \
    outliner-marshal.c \
    outliner-window.c \
    outliner-window.h \
    outliner-actions.c \
    outliner-actions.h \
    outliner-view.c \
    outliner-view.h \
    outliner-document.c \
    outliner-document.h \
    outliner-file.c \
    outliner-file.h \
    outliner-fprops-dialog.c \
    outliner-fprops-dialog.h \
    outliner-opml.c \
    outliner-opml.h \
    xml-gnomevfs.c \
    xml-gnomevfs.h \
    outliner-cell-renderer.c \
    outliner-cell-renderer.h \
    outliner-text-view.c \
    outliner-text-view.h \
    outliner-main.c

gnome_outliner_LDADD = $(DEPS_LIBS)

AM_CPPFLAGS = $(DEPS_CFLAGS) \
              -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\" \
              -DPACKAGE_DATA_DIR=\""$(pkgdatadir)/"\"

# Signal Marshallers

GENMARSHAL_FLAGS = --prefix="outliner_marshal" --g-fatal-warnings

outliner-marshal.h: outliner-marshal.list
	@GLIB_GENMARSHAL@ $(GENMARSHAL_FLAGS) \
		--header outliner-marshal.list > outliner-marshal.h

outliner-marshal.c: outliner-marshal.h
	@GLIB_GENMARSHAL@ $(GENMARSHAL_FLAGS) \
		--body outliner-marshal.list > outliner-marshal.c

EXTRA_DIST = outliner-marshal.list
CLEANFILES = outliner-marshal.c outliner-marshal.h
