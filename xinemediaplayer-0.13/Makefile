#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = xinemediaplayer

### The object files (add further files here):

OBJS = $(PLUGIN).o Control.o Player.o Reel.o SpuDecode.o Utils.o \
		curl_download.o imagetools.o \
		item_tree.o  m3u_parser.o pls_parser.o timecounter.o \
		xineOsd.o xineOsdInfoMenu.o xineOsdFunction.o Playlist.o

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

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):
GSTREAMER10 = 1

ifdef RBMINI
  DEFINES += -DUSE_PLAYPES
  OBJS    += InputThread.o PesMpaTools.o mpa_file_length.o
  LIBS    += -lmagic
else
  OBJS += cddb.o cdtext.o cdrom.o
  OBJS += XineLib.o GstreamerLib.o gstTools.o ExternalLib.o
ifndef GSTREAMER10
  LIBS += -lxine `pkg-config --libs gstreamer-0.10` `pkg-config --libs gstreamer-interfaces-0.10`
  INCLUDES += `pkg-config --cflags gstreamer-0.10`  `pkg-config --cflags gstreamer-interfaces-0.10`
else
  LIBS += -lxine `pkg-config --libs gstreamer-1.0`
  INCLUDES += `pkg-config --cflags gstreamer-1.0`
  DEFINES += -DGSTREAMER10
endif
endif

### common Makefile rules
-include Make.common

INCLUDES +=

ifdef RBLITE
  ### paths to xine, only neccessary if not installed locally
  LIBXINE      ?= ../../../../temp/docimage/libs/xine-lib/src/xine-engine
  XINE_INCLUDE ?= ../../../../temp/docimage/libs/xine-include

  LIBCDIO  ?= ../../../../temp/docimage/libs/libcdio
  INCLUDES += -I$(XINE_INCLUDE) -I$(LIBCDIO)/include
  LIBS     += -L$(LIBXINE)/.libs -L$(LIBCDIO)/lib/driver/.libs
endif

ifdef RBMINI
  LDFLAGS  += -ltag -lcurl
  INCLUDES += -I/usr/arm-linux-gnueabi/include/taglib
else
  INCLUDES +=`taglib-config --cflags`
  LDFLAGS  +=`taglib-config --libs`
  LIBS     += `curl-config --libs` -lcdio -lcddb
endif

DEFINES += -DPLAYER_VERSION=\"$(PLAYER_VERSION)\" -D__LINUX__ -D__STL_CONFIG_H


#target-for-compatibility-with-vanilla-vdr:
#	$(LIBDIR)/$@.$(APIVERSION)
