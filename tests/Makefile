include ../src/Makefile.inc 

PARTS=fs xtime template_parser executor xregex xlogger xstring
SOURCES.cc = run_tests.cc 
DEPSRC=$(addsuffix .cc, $(PARTS))
MAINSRC.cc = $(shell ls -1 ../src/*/*.h | xargs) \
	     $(shell ls -1 ../src/*/*.cc | xargs)

LIBOBJ = ../src/libtools.a
SOURCES = $(SOURCES.cc) $(LIBOBJ)

PROGRAM = run_tests

INCS = -I../src -I. -I/usr/include/libdwarf -I/usr/include/elfutils 
LIBS = -L../src -ltools $(LDLIBS) -L../src/libtools.a 
#LIBS = ../libtools.lin64.a -ldwarf -ldw -lm -lelf -pthread 

$(PROGRAM): $(SOURCES.cc) 
	$(CPP) $(INCS) $(CCFLAGS) $(LDFLAGS) -o $@ $(SOURCES) $(LIBS)

$(PROGRAM): $(DEPSRC)

clean:
	$(RM) $(PROGRAM)

# build with optimization CCFLAGS 
optimize: CCFLAGS=$(FAST_CCFLAGS)
optimize: $(PROGRAM)

# run all tests
tests: $(PROGRAM)
	./run_tests 

# generate ctags file
tags:
	ctags --fields=+l $(shell find . -name "*.h" -or -name "*.cc")


# format sourcecode with clang-format
format:
	clang-format -i -style=file *.cc
