top_srcdir = 
top_builddir = 
this_srcdir = 

all:

# Don't include the config file if we're trying to make it
ifneq ($(MAKECMDGOALS),config)
include config.mk
endif

include Makefile.local
include makefiles/Makefile.common

all-nonrecursive: config.mk

AUTOCONFIG = tools/autogenerate-config.sh

config.mk:
	@echo
	@echo "*** If you haven't looked at INSTALL yet, now might be a good time to"
	@echo "*** hit Ctrl+C and do so."
	@echo "*** Otherwise, best guess build will proceed in 2 seconds..."
	@echo "*** (This message will only appear if config.mk is absent)"
	@echo
	sleep 2
	$(AUTOCONFIG)

config:
	$(AUTOCONFIG)

#####################################################
#
#  Alternate build dir generation
#
#####################################################

BUILTIN_CONFIG_MK_SOURCES := $(wildcard makefiles/config.mk.*)
BUILTIN_CONFIGS := \
	$(patsubst makefiles/config.mk.%,%,$(BUILTIN_CONFIG_MK_SOURCES))
BUILTIN_CONFIG_MK_DESTS := $(addsuffix /config.mk,$(BUILTIN_CONFIGS))
BUILTIN_CONFIG_MAKEFILES := $(addsuffix /Makefile,$(BUILTIN_CONFIGS))

$(BUILTIN_CONFIG_MK_DESTS): %/config.mk: makefiles/config.mk.%
	@mkdir -p $*
	ln -s ../$< $*/config.mk

$(BUILTIN_CONFIG_MAKEFILES): %/Makefile: $(top_srcdir)makefiles/Makefile.common
	@mkdir -p $*
	@echo '# This file is autogenerated.  Do not edit it' > $@
	@echo 'top_srcdir = ../' >> $@
	@echo 'top_builddir = ' >> $@
	@echo 'this_srcdir = ../' >> $@
	@echo 'this_srcdir = ../' >> $@
	@echo >> $@
	@echo 'all:' >> $@
	@echo >> $@
	@echo 'include $$(top_builddir)config.mk' >> $@
	@echo 'include ../Makefile.local' >> $@
	@echo 'include $$(top_srcdir)makefiles/Makefile.common' >> $@

$(BUILTIN_CONFIGS): %: %/Makefile %/config.mk
	$(MAKE) -C $* all

#####################################################
#
#  docs
#
#####################################################

doc:
	doxygen Doxyfile

.PHONY: config doc

