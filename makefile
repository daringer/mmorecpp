include $(AMV_HOME)/templates/makefile.libs
include $(AMV_HOME)/templates/makefile$(EXTENSION_LONG)

LIB = libtools$(EXTENSION).a

PARTS = config_manager exception xstring xsocket fs template_parser xregex io testing_suite executor xlogger xtime threading mem_tracker
SOURCES.cc = $(addsuffix .cc,$(PARTS))
SOURCES.h = $(addsuffix .h,$(PARTS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%$(EXTENSION).o)

CCFLAGS = -gdwarf-2 -w -I.. -std=c++11

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	# Removing libs from $(LAPACKDIR_OHNE_PERF)!!!
	$(RM) $(OBJECTS) core $(LIB)

.PARALLEL: $(OBJECTS)


# building object files here
%$(EXTENSION).o: %.cc $(SOURCES.h) $(MAKEFILE) $(MAKEFILEINC)
	@rm -f $@
	$(CCC) $(CCFLAGS) -c -g $(@:%$(EXTENSION).o=%.cc) -o $@
	@chmod g+w,a+r $@

# building/linking library 
$(LIB): $(SOURCES) $(OBJECTS) $(MAKEFILE) $(MAKEFILEINC)
	# Compiling $(LIB)
	@rm -f $(LIB)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)

incl_file: $(INCL_LIB)

include makefile.inc
