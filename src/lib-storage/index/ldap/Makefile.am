noinst_LTLIBRARIES = libstorage_ldap.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/lib \
	-I$(top_srcdir)/src/lib-settings \
	-I$(top_srcdir)/src/lib-mail \
	-I$(top_srcdir)/src/lib-imap \
	-I$(top_srcdir)/src/lib-index \
	-I$(top_srcdir)/src/lib-storage \
	-I$(top_srcdir)/src/lib-storage/index

libstorage_ldap_la_SOURCES = \
	ldap-copy.c \
	ldap-mail.c \
	ldap-settings.c \
	ldap-storage.c \
	ldap-sync.c

headers = \
	ldap-settings.h \
	ldap-storage.h \
	ldap-sync.h

pkginc_libdir=$(pkgincludedir)
pkginc_lib_HEADERS = $(headers)