sieve_plugindir = $(dovecot_moduledir)/sieve

sieve_plugin_LTLIBRARIES = lib90_sieve_proton_compat_plugin.la

lib90_sieve_proton_compat_plugin_la_LDFLAGS = -module -avoid-version

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(LIBDOVECOT_INCLUDE) \
	$(LIBSIEVE_INCLUDE) \
	-DPKG_RUNDIR=\""$(rundir)"\"

commands = \
	cmd-expire.c \
	cmd-unexpire.c

tests = \
	tst-expiration.c \
	tst-hasexpiration.c

extensions = \
	ext-vnd-proton-expire.c

lib90_sieve_proton_compat_plugin_la_SOURCES = \
	$(commands) \
	$(tests) \
	$(extensions) \
	sieve-proton-expiration.c \
	sieve-proton-compat-plugin.c

lib90_sieve_proton_compat_plugin_la_LIBADD =

noinst_HEADERS = \
	sieve-proton-expiration.h \
	sieve-proton-compat-common.h \
	sieve-proton-compat-plugin.h
