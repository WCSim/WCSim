# $Id: GNUmakefile,v 1.17 2006/09/04 15:43:27 t2k Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------


G4DEBUG = 1

name := testWCSim
G4TARGET := $(name)
TESTFILES := $(wildcard *.hh)

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

ROOTCFLAGS   := $(shell root-config --cflags) -DUSE_ROOT -fPIC
ROOTLIBS     := $(shell root-config --libs)

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

CPPFLAGS  += -I. -I../include -I$(ROOTSYS)/include $(ROOTCFLAGS) 
EXTRALIBS += $(ROOTLIBS) $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/libWCSim.a

EXTRA_LINK_DEPENDENCIES := 

.PHONY: test
test: testWCSim	

testWCSim: testWCSim.cc $(G4WORKDIR)/tmp/$(G4SYSTEM)/WCSim/libWCSim.a
	g++ $(CPPFLAGS) -o testWCSim testWCSim.cc -L$(G4LIBDIR) -L$(CLHEP_LIB_DIR) $(LDLIBS) $(EXTRALIBS)

testWCSim.cc: $(TESTFILES)
	./cxxtestgen.py --error-printer -o testWCSim.cc $(TESTFILES)

include $(G4INSTALL)/config/binmake.gmk
