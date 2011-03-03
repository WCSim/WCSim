# $Id: GNUmakefile,v 1.17 2006/09/04 15:43:27 t2k Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

G4DEBUG = 1

name := WCSim
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

ROOTCFLAGS   := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLIBS     := $(shell root-config --libs)

LIBNAME := WCSim

# NOTE: Geant4.7.0 changes the way Maximum Step size is defined.  
# We need extra code for versions 4.7.0 and above; eventually 
# everyone should upgrade to geant4.7
ifneq (,$(findstring 4.7,$(G4INSTALL)))
GEANT4_7_0 = 0
else
GEANT4_7_0 = 1
endif

ifdef GEANT4_7_0
CPPFLAGS += -DGEANT4_7_0
endif

ifdef GCCVERS296
CPPFLAGS += -DUSE_STRSTREAM
endif

CPPFLAGS  += -I$(ROOTSYS)/include $(ROOTCFLAGS) 
EXTRALIBS += $(ROOTLIBS)

EXTRA_LINK_DEPENDENCIES := 

.PHONY: all test
all: rootcint lib bin shared libWCSim.a

# Note dependencies not yet set up right yet

ROOTSO    := libWCSimRoot.so

ROOTSRC  := ./src/WCSimRootEvent.cc ./include/WCSimRootEvent.hh ./src/WCSimRootGeom.cc ./include/WCSimRootGeom.hh ./include/WCSimPmtInfo.hh ./include/WCSimRootLinkDef.hh

ROOTOBJS  := $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/WCSimRootEvent.o $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/WCSimRootGeom.o $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/WCSimPmtInfo.o $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/WCSimRootDict.o 

shared: $(ROOTSRC) $(ROOTOBJS) 
	g++ -shared -O $(ROOTOBJS) -o $(ROOTSO) $(ROOTLIBS)

libWCSim.a : $(ROOTOBJS)
	$(RM) $@
	ar clq $@ $(ROOTOBJS) 

./src/WCSimRootDict.cc : $(ROOTSRC)
	rootcint  -f ./src/WCSimRootDict.cc -c -I./include -I$(shell root-config --incdir) WCSimRootEvent.hh WCSimRootGeom.hh  WCSimPmtInfo.hh WCSimRootLinkDef.hh

rootcint: ./src/WCSimRootDict.cc

test: $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/libWCSim.a
	$(MAKE) -C test

include $(G4INSTALL)/config/binmake.gmk
