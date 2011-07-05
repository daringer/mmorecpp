include $(AMV_HOME)/templates/makefile.libs
include $(AMV_HOME)/templates/makefile$(EXTENSION_LONG)

#LIB = libtools$(EXTENSION).a

TESTSUITS = fs xstring
SUITESOURCES.cc = $(addsuffix .cc,$(TESTSUITS))
SOURCES.cc = run_tests.cc 
#SOURCES.h = $(addsuffix .h,$(TOOLS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%$(EXTENSION).o)

CCFLAGS += -w -I.. -g 
PROGRAM = run_tests

LIBS += -lm -ltools$(EXTENSION) -L$(AMV_HOME)/c/tools_lib


all:	 $(PROGRAM)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	# Removing libs from $(LAPACKDIR_OHNE_PERF)!!!
	$(RM) $(OBJECTS) core $(LIB)

.PARALLEL: $(OBJECTS)

$(PROGRAM): $(SOURCES) $(OBJECTS) $(OBJVHDL) $(MAKEFILE) $(MAKEFILEINC) $(SUITESOURCES.cc)
	@echo "\n - Linking $(PROGRAM) -\n"
	$(LINK_PRE) $(LD) $(LDFLAGS)  -o $@ $(OBJECTS) $(OBJVHDL) $(LIBS)
	@chmod 755 $@

# building object files here
%$(EXTENSION).o: %.cc $(SOURCES.h) $(MAKEFILE) $(MAKEFILEINC)
	@rm -f $@
	$(CCC) $(CCFLAGS) -c -g $(@:%$(EXTENSION).o=%.cc) -o $@
	@chmod g+w,a+r $@

incl_file: $(INCL_LIB)
