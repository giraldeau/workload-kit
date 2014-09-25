ACLOCAL_AMFLAGS = -I config

# FIXME: copy everything under scripts
script_dir_u := scripts/ust
files_u := $(foreach script,$(script_dir_u),$(wildcard $(script)/*))
script_dest_u := $(DESTDIR)$(pkgdatadir)/$(script_dir_u)

script_dir_k := scripts/kernel
files_k := $(foreach script,$(script_dir_k),$(wildcard $(script)/*))
script_dest_k := $(DESTDIR)$(pkgdatadir)/$(script_dir_k)

if HAVE_UST
  USTDIR = ust
endif

script_dir_p := scripts/profiles
files_p := $(foreach script,$(script_dir_p),$(wildcard $(script)/*))
script_dest_p := $(DESTDIR)$(pkgdatadir)/$(script_dir_p)

SUBDIRS = src utils $(USTDIR) tests cluster
EXTRA_DIST = $(script_dir_u) $(script_dir_k) $(script_dir_p) doc utils

install-data-local:
	test -z "$(script_dest_u)" || $(MKDIR_P) "$(script_dest_u)"
	test -z "$(script_dest_k)" || $(MKDIR_P) "$(script_dest_k)"
	test -z "$(script_dest_p)" || $(MKDIR_P) "$(script_dest_p)"
	$(INSTALL_SCRIPT) -m 0755 $(files_u) "$(script_dest_u)"
	$(INSTALL_SCRIPT) -m 0755 $(files_k) "$(script_dest_k)"
	$(INSTALL_SCRIPT) -m 0644 $(files_p) "$(script_dest_p)"

uninstall-local:
	rm -rf "$(script_dest_u)"
	rm -rf "$(script_dest_k)"
	rm -rf "$(script_dest_p)"


# requires gnulib
ChangeLog:
	if test -d $(top_srcdir)/.git; then                             \
          sh /usr/share/gnulib/build-aux/gitlog-to-changelog > $@;             \
        fi

dist: ChangeLog

.PHONY: ChangeLog


