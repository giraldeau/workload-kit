ACLOCAL_AMFLAGS = -I config

script_dir_u := scripts/ust
files_u := $(foreach script,$(script_dir_u),$(wildcard $(script)/*))
script_dest_u := $(DESTDIR)$(pkgdatadir)/$(script_dir_u)

script_dir_k := scripts/kernel
files_k := $(foreach script,$(script_dir_k),$(wildcard $(script)/*))
script_dest_k := $(DESTDIR)$(pkgdatadir)/$(script_dir_k)

SUBDIRS = src utils ust tests
EXTRA_DIST = $(script_dir_u) $(script_dir_k) doc utils

install-data-local:
	test -z "$(script_dest_u)" || $(MKDIR_P) "$(script_dest_u)"
	test -z "$(script_dest_k)" || $(MKDIR_P) "$(script_dest_k)"
	$(INSTALL_SCRIPT) -m 0755 $(files_u) "$(script_dest_u)"
	$(INSTALL_SCRIPT) -m 0755 $(files_k) "$(script_dest_k)"

uninstall-local:
	rm -rf "$(script_dest_u)"
	rm -rf "$(script_dest_k)"


