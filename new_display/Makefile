## T.J. Corona, 2005
## based on makefile by M. Fechner, 2003

## -lNew must be included first if ROOT is to be used
## with STL containers; otherwise runtime errors occur with
## cryptic messages about failure in operator new, cf roottalk. 

# should test if Fedora Core 4 ; use 3.2 versions if FC4 unless
# you compiled ROOT with g++ 4.0 

ISFC = $(shell if [ -e /etc/fedora-release ]; then \
	cat /etc/fedora-release; \
	fi;)

CXX=g++
CC=gcc

ROOTCFLAGS   = $(shell root-config --cflags)
ROOTMAIN     = $(shell root-config --libs)
ROOTGMAIN     = $(shell root-config --glibs)
ROOTLIBS     = -L$(ROOTSYS)/lib -lNew $(ROOTMAIN) $(ROOTGMAIN) -lMinuit
CERNLIB = `cernlib pawlib graflib packlib mathlib kernlib` -lg2c -lm
G42KMWORKDIR = ../
JHF2KMLIB = $(G42KMWORKDIR)/libWCSimRoot.so -L$(G42KMWORKDIR) -lWCSim
LDLIBS +=  $(JHF2KMLIB) $(ROOTLIBS)

ODIR = ./objects

$(ODIR)/%.o: %.cc
	mkdir -p $(ODIR)
	$(CXX) $(ROOTCFLAGS) -fPIC -I$(G42KMWORKDIR)/include -c -o $@ $<

$(ODIR)/%.o: %.C
	mkdir -p $(ODIR)
	$(CXX) $(ROOTCFLAGS) -fPIC -c  -o $@ $<

OBJS = $(ODIR)/MCEvent.o  $(ODIR)/WCMapDict.o $(ODIR)/Xin_Draw.o

OBJSMAIN = 

SRC = MCEvent.cc MCEvent.h  Xin_Draw.cc Xin_Draw.h WCMapLinkDef.h 

SO = libWCMap.so

standard: rootcint shared

rootcint: $(SRC)
	rootcint -f WCMapDict.cc -c -I. -I$(G42KMWORKDIR)/include MCEvent.h Xin_Draw.h WCMapLinkDef.h

shared: $(SRC) $(OBJS)
	$(CXX) -shared -O $(OBJS) -o $(SO) $(ROOTLIBS)

clean:
	rm -f $(ODIR)/*.o *.exe *~ *.d 

realclean: clean
	rm -f *Dict.cc *Dict.h


all:   	rootcint shared 
