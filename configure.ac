AC_PREREQ(2.61)
AC_INIT([fat-label], [0.3])
AC_CONFIG_SRCDIR([src/lib_bg90_fat_label.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([src/config.h])

AC_PROG_CC
AC_HEADER_STDC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(FAT_LABEL, hildon-1 libhildondesktop-1 gtk+-2.0)
AC_SUBST(FAT_LABEL_CFLAGS)
AC_SUBST(FAT_LABEL_LIBS)

# Discover where to install the .desktop file:
HILDON_HOME_DESKTOP_DIR=`pkg-config libhildondesktop-1 --variable=hildonhomedesktopentrydir`
AC_SUBST(HILDON_HOME_DESKTOP_DIR)

# Discover where to install the status widget's lib file:
HILDON_DESKTOP_LIB_DIR=`pkg-config libhildondesktop-1 --variable=hildondesktoplibdir`
AC_SUBST(HILDON_DESKTOP_LIB_DIR) 


AC_OUTPUT(Makefile)
AC_OUTPUT(src/Makefile)
AC_OUTPUT(src/bg90_fat_label.desktop)

