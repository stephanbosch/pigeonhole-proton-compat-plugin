AC_PREREQ([2.69])
m4_define([pigeonhole_proton_compat_plugin_VERSION],
    m4_esyscmd_s([build-aux/git-version-gen]))



AC_INIT([dovecot-pigeonhole-proton-compat-plugin], [m4_defn([pigeonhole_proton_compat_plugin_VERSION])], [dovecot@dovecot.org])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([no-define foreign tar-ustar])
AM_SILENT_RULES([yes])

# Autoheader is not needed and does more harm than good for this package.
# However, it is tightly integrated in autoconf/automake and therefore it is
# difficult not to use it. As a workaround we give autoheader a dummy config
# header to chew on and we handle the real config header ourselves.
AC_CONFIG_HEADERS([dummy-config.h sieve-proton-compat-config.h])

AC_DEFINE_UNQUOTED(SIEVE_PROTON_COMPAT_NAME, "$PACKAGE_NAME",
	[Define to the full name of this plugin implementation.])
AC_DEFINE_UNQUOTED(SIEVE_PROTON_COMPAT_VERSION, "$PACKAGE_VERSION",
	[Define to the version of this plugin implementation.])

AM_MAINTAINER_MODE

LT_INIT

DC_DOVECOT
DC_DOVECOT_MODULEDIR

LIBDOVECOT_INCLUDE="$LIBDOVECOT_INCLUDE $LIBDOVECOT_STORAGE_INCLUDE"
CFLAGS="$CFLAGS $EXTRA_CFLAGS"
LIBS="$DOVECOT_LIBS"
AC_SUBST(LIBDOVECOT_INCLUDE)

DC_PIGEONHOLE

AC_ARG_WITH(docs,
[  --with-docs             Install documentation (default)],
    if test x$withval = xno; then
        want_docs=no
    else
        want_docs=yes
    fi,
    want_docs=yes)
AM_CONDITIONAL(BUILD_DOCS, test "$want_docs" = "yes")

AS_IF([test "$AR_FLAGS" = "cru"], [AR_FLAGS="cr"], [])
AC_SUBST([AR_FLAGS])

AC_CONFIG_FILES([
Makefile
src/Makefile
doc/Makefile
build-aux/run-test.sh
stamp.h])

AC_OUTPUT
