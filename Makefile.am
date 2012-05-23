ACLOCAL_AMFLAGS = -I config

script_dir := scripts
files := $(foreach script,$(script_dir),$(wildcard $(script)/*))
script_dest := $(DESTDIR)$(pkgdatadir)/$(script_dir)

SUBDIRS = src utils ust
EXTRA_DIST = $(script_dir) doc utils

install-data-local:
	test -z "$(script_dest)" || $(MKDIR_P) "$(script_dest)"
	$(INSTALL_SCRIPT) -m 0755 $(files) "$(script_dest)"

uninstall-local:
	rm -rf "$(script_dest)"


