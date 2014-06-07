CCC = clang++ #g++
AR = ar
RANLIB = ranlib
RM = rm -f

#PARTS=fs xtime template_parser
#PARTS=
SOURCES.cc = run_tests.cc #$(addsuffix .cc,$(PARTS))

LIBOBJ = ../src/libtools.a
SOURCES = $(SOURCES.cc) $(LIBOBJ)

CCFLAGS = -w -g -Wall -pedantic -rdynamic -std=c++0x
PROGRAM = run_tests
LDFLAGS = -g -rdynamic 

INCS = -I../src -I. -I/usr/include/libdwarf -I/usr/include/elfutils
LIBS = -L../src -ltools -lm -pthread -L../src/libtools.a
#LIBS = ../libtools.lin64.a -ldwarf -ldw -lm -lelf -pthread 


all:	 $(PROGRAM)

clean:
	$(RM) $(PROGRAM)

$(PROGRAM): $(SOURCES.cc)
	$(CCC) $(INCS) $(CCFLAGS) $(LDFLAGS) -o $@ $(SOURCES) $(LIBS)

# run all tests
tests: $(PROGRAM)
	./run_tests 

# format sourcecode with clang-format
format:
	clang-format -i -style=file *.cc
