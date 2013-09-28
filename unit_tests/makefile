CCC = g++
AR = ar
RANLIB = ranlib
RM = rm -f

TESTSUITS = fs xstring xregex executor mem_tracker
SOURCES.cc = run_tests.cc $(addsuffix .cc,$(TESTSUITS))

SOURCES = $(SOURCES.cc)
LIBOBJ =  ../libtools.a

CCFLAGS = -w -I. -I.. -ggdb -g -Wall -pedantic -rdynamic -std=c++0x
PROGRAM = run_tests
LDFLAGS = -ggdb -rdynamic -g -w

LIBS = -lm

all:	 $(PROGRAM)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	$(RM) $(OBJECTS) $(PROGRAM)

$(PROGRAM): $(SOURCES) 
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o $@ $(SOURCES)

#%.o: $(SOURCES)
#	$(CCC) $(CCFLAGS) $(LDFLAGS) -c -g $(@:%.o=%.cc) -o $@ 
