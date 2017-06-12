#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = bgprocess

### The object files (add further files here):

OBJS = bgprocess.o 


### The C++ compiler and options:

#CXX      ?= g++
#CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual
#LDFLAGS  += -O3 -Wl,--retain-symbols-file,retain-sym
### The directory environment:

#VDRDIR = ../../..
#LIBDIR = ../../lib
# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR = $(DESTDIR)$(call PKGCFG,libdir)
LOCDIR = $(DESTDIR)$(call PKGCFG,locdir)

TMPDIR ?= /tmp

### Allow user defined options to overwrite defaults:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

#-include $(PLGCFG)
#-include $(VDRDIR)/Make.config

### The name of the shared object file:
SOFILE = libvdr-$(PLUGIN).so


#APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES +=

### Aditional rules
#i18n.c: i18n-template.c po2i18n.pl $(I18Npo)
#	./po2i18n.pl < $< > $@

#clean: clean-plugin
#clean-plugin:
#	@-rm -f i18n.c

#libvdr-$(PLUGIN).so: $(OBJS) retain-sym
#	$(CXX) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@
#	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

#retain-sym:
#	echo "VDRPluginCreator" > retain-sym
include Make.common
