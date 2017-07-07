LIBDIR        = lib
SRCDIR        = src
INTDIR        = include
CINTDIR       = cint

include $(ROOTSYS)/etc/Makefile.arch

CXXFLAGS += -I../.. -I.

ifeq ($(PLATFORM),macosx)
CXXFLAGS += -std=c++11
endif

OBJS = $(LIBDIR)/Plotter.o $(LIBDIR)/StyleInfo.o $(LIBDIR)/PlotTools.o $(LIBDIR)/Drawable.o $(LIBDIR)/Drawing.o    HistoPlottingDict.o
INTS = $(INTDIR)/Plotter.h $(INTDIR)/PlotTools.h $(INTDIR)/Drawable.h $(INTDIR)/Drawing.h $(INTDIR)/StyleInfo.h

libHistoPlotting.so: $(OBJS)
ifeq ($(PLATFORM),macosx)
	$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif

HistoPlottingDict.cc: $(INTS) $(SRCDIR)/HistoPlotting_LinkDef.h
	@echo "Generating dictionary $@..."
	rootcling  -v -l -f $@ -c -I../.. $^ 
$(LIBDIR)/HistoPlottingDict.o: HistoPlottingDict.cc
	$(CXX)  $(CXXFLAGS) -c $<  -o $@	

$(LIBDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $<  -o $@

all:  libHistoPlotting.so
	
# =============================== 
clean: 
	rm -f $(LIBDIR)/* $(CINTDIR)/* HistoPlottingDict* libHistoPlotting.so
# =============================== # DO NOT DELETE
# DO NOT DELETE
