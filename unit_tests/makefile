include $(AMV_HOME)/templates/makefile.libs
include $(AMV_HOME)/templates/makefile$(EXTENSION_LONG)



TESTSUITS = fs xstring xregex executor
SOURCES.cc = run_tests.cc $(addsuffix .cc,$(TESTSUITS))

SOURCES = $(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%$(EXTENSION).o)
LIBOBJ =  ../libtools$(EXTENSION).a

CCFLAGS = -w -I. -I.. -g -Wall -pedantic -DLINUX -I$(TOOLS_DIR)
PROGRAM = run_tests

LIBS += -lm -ltools$(EXTENSION) -L$(AMV_HOME)/c/tools_lib

CCC=clang

all:	 $(PROGRAM)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	$(RM) $(OBJECTS) core $(LIB)

.PARALLEL: $(OBJECTS)

$(LIBOBJ): 
	make -C ..

$(PROGRAM): $(SOURCES) $(OBJECTS) $(OBJVHDL) $(MAKEFILE) $(MAKEFILEINC) $(LIBOBJ)
	@echo "\n - Linking $(PROGRAM) -\n"
	$(LINK_PRE) $(LD) $(LDFLAGS)  -o $@ $(OBJECTS) $(OBJVHDL) $(LIBS)
	@chmod 755 $@

# building object files here
%$(EXTENSION).o: %.cc $(SOURCES.h) $(MAKEFILE) $(MAKEFILEINC) $(LIBOBJ)
	@rm -f $@
	$(CCC) $(CCFLAGS) -c -g $(@:%$(EXTENSION).o=%.cc) -o $@
	@chmod g+w,a+r $@

incl_file: $(INCL_LIB)
