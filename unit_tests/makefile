include $(AMV_HOME)/templates/makefile.libs
include $(AMV_HOME)/templates/makefile$(EXTENSION_LONG)



TESTSUITS = fs xstring xregex executor
SOURCES.cc = run_tests.cc $(addsuffix .cc,$(TESTSUITS))

SOURCES = $(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%$(EXTENSION).o)
LIBOBJ =  ../libtools$(EXTENSION).a

CCFLAGS = -w -c -I. -I.. -ggdb -g -Wall -pedantic -DLINUX -I$(TOOLS_DIR) -rdynamic -std=c++0x
#CCFLAGS = -w -c -I. -I.. -ggdb -g -Wall -pedantic -DLINUX -I$(TOOLS_DIR) -rdynamic
PROGRAM = run_tests
LDFLAGS = -ggdb -rdynamic -g -w

LIBS += -lm -ltools$(EXTENSION) -L$(AMV_HOME)/c/tools_lib

all:	 $(PROGRAM)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	$(RM) $(OBJECTS) core $(LIB)

.PARALLEL: $(OBJECTS)

$(PROGRAM): $(SOURCES) $(OBJECTS) $(OBJVHDL) $(MAKEFILE) $(MAKEFILEINC)
	@echo "\n - Linking $(PROGRAM) -\n"
	$(LINK_PRE) $(LD) $(LDFLAGS)  -o $@ $(OBJECTS) $(OBJVHDL) $(LIBS)
	@chmod 755 $@

# building object files here
%$(EXTENSION).o: %.cc $(SOURCES.h) $(MAKEFILE) $(MAKEFILEINC) $(LIBOBJ)
	@rm -f $@
	$(CCC) $(CCFLAGS) -c -g $(@:%$(EXTENSION).o=%.cc) -o $@
	@chmod g+w,a+r $@

incl_file: $(INCL_LIB)
