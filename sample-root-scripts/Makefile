CPP 		= g++
CXXFLAGS	= -g -O3 -Wall -fPIC -D_REENTRANT -Wno-deprecated -fpermissive

ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS     	:= $(shell root-config --libs)
ROOTGLIBS    	:= $(shell root-config --glibs)
CXXFLAGS	+= $(ROOTCFLAGS)

#LIB = /home/bquilain/CC0pi_XS/MC/ingmc_IngridRevReWeightFinal/lib_New
LIB = lib_New_r34
#lib
#export WCSIMDIR=/disk01/usr5/bquilain/WCSim_hybridPMT_official
LIBS 		= $(ROOTGLIBS) $(WCSIMDIR)/libWCSimRoot.so -lMinuit
#LIBS 		= $(ROOTGLIBS) /disk01/usr5/bquilain/WCSim_hybridPMT_official/libWCSimRoot.so -lMinuit

INC = $(WCSIMDIR)/include
SRC= $(WCSIMDIR)/src

CXXFLAGS += -I$(LIB) -I$(SRC) -I$(INC) 

TARGET= AnalyzeWSHierarchy

all: $(TARGET)
AnalyzeWSHierarchy: AnalyzeWSHierarchy.o


%: %.o
	@echo "Now make $@"
	@$(CPP) -o $@ $< $(CXXFLAGS) $(LIBS) 
	@echo "..Compile done! "

%.o: %.c
	@echo "$<"
	@echo "Start Compiling $<"
	@$(CPP) $(CXXFLAGS) -c $<
	@echo ".. Compiling Object Files $<   --> done"
	@echo ""

clean: 
	@echo "Now Clean Up"
	rm -f $(TARGET) *~ *.o *.o~ core
