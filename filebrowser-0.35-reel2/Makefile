#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = filebrowser

### The object files (add further files here):

OBJS = filebrowser.o mountmanager.o menuBrowserBase.o menuBrowser.o fileType.o dirType.o \
	stillpictureplayer.o convert.o filecache.o menufileinfo.o browserItems.o \
	threadprovider.o menuplaylist.o playlist.o playlistUserIf.o browserStatus.o \
	userIf.o userIfBase.o filetools.o setup.o menuDirSelect.o

### The C++ compiler and options:

#CXX      ?= g++
#CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

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

DEFINES  += -D__STL_CONFIG_H

ifdef RBMINI
INCLUDES += -I. -I/usr/arm-linux-gnueabi/include/taglib
LDFLAGS += -L/usr/arm-linux-gnueabi/lib -ltag 
else
INCLUDES += -I. -I/usr/include/taglib

INCLUDES +=`taglib-config --cflags`
LDFLAGS  +=`taglib-config --libs`
endif

#$(I18Npot): $(PLUGIN).h

#target-for-compatibility-with-vanilla-vdr:        
#	$(LIBDIR)/$@.$(APIVERSION)

include Make.common
