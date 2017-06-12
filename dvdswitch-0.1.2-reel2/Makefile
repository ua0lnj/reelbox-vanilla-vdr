#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = dvdswitch

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The object files (add further files here):

OBJS = $(PLUGIN).o debug.o helpers.o tools.o imagelist-item.o imagelist.o dvdlist-item.o \
	dvdlist.o  menu-item.o menu.o setup.o setup-itypes.o commands.o xineplugin.o


### The directory environment:

#VDRDIR = ../../..
#LIBDIR = ../../lib
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR = $(DESTDIR)$(call PKGCFG,libdir)
LOCDIR = $(DESTDIR)$(call PKGCFG,locdir)



TMPDIR ?= /tmp

### Allow user defined options to overwrite defaults:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### The name of the shared object file:
SOFILE = libvdr-$(PLUGIN).so

#-include $(VDRDIR)/Make.config
include Make.common


#target-for-compatibility-with-vanilla-vdr:                                                      
#	$(LIBDIR)/$@.$(APIVERSION)
