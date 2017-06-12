#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.2 2004/11/21 17:51:20 lordjaxom Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = mediad

#DEBUG=1

HALCXXFLAGS  = `pkg-config --cflags hal`
DBUSCXXFLAGS = `pkg-config --cflags dbus-1`
GLIBCXXFLAGS = `pkg-config --cflags glib-2.0 dbus-glib-1`
HALLIBS      = `pkg-config --libs hal`
DBUSLIBS     = `pkg-config --libs dbus-1`
GLIBLIBS     = `pkg-config --libs glib-2.0 dbus-glib-1`

### The object files (add further files here):
OBJS = $(PLUGIN).o setup.o status.o plugins.o mediaplugin.o hal.o \
	selectdevicemenu.o devicesetup.o suspend.o optionsmenu.o

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


#-include $(VDRDIR)/Make.config
### The name of the shared object file:
SOFILE = libvdr-$(PLUGIN).so


### Includes and Defines (add further entries here):

INCLUDES += $(DBUSCXXFLAGS) $(GLIBCXXFLAGS) $(HALCXXFLAGS)

DEFINES  += -DDBUS_API_SUBJECT_TO_CHANGE


ifeq ($(shell test -f $(VDRDIR)/PLUGINS/src/mp3/mp3.h ; echo $$?),0)
  DEFINES += -DHAVE_MP3_H
endif

ifeq ($(shell test -f $(VDRDIR)/PLUGINS/src/mp3/mplayer.h ; echo $$?),0)
  DEFINES += -DHAVE_MPLAYER_H
endif

DEFINES += -DHAVE_XINEMEDIAPLAYER

LDFLAGS += $(DBUSLIBS) $(GLIBLIBS) $(HALLIBS)

#target-for-compatibility-with-vanilla-vdr:
#	$(LIBDIR)/$@.$(APIVERSION)
include Make.common
