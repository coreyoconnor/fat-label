# We invent desktoplib_LTLIBRARIES instead of the normal lib_LTLIBRARIES,
# so we can specify the non-standard installation directory.
desktoplib_LTLIBRARIES =  lib_bg90_fat_label.la
desktoplibdir = $(HILDON_DESKTOP_LIB_DIR)

lib_bg90_fat_label_la_SOURCES = lib_bg90_fat_label.c lib_bg90_fat_label.h
lib_bg90_fat_label_la_LIBADD = $(FAT_LABEL_LIBS)

AM_CFLAGS = -Wall $(FAT_LABEL_CFLAGS)

# .desktop file
desktopdir = $(HILDON_HOME_DESKTOP_DIR)
desktop_DATA = bg90_fat_label.desktop

DISTCLEANFILES = $(desktop_DATA)

