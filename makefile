
LIB = libtools.a

PARTS = config_manager exception xstring xsocket fs template_parser xregex io testing_suite executor xlogger xtime threading mem_tracker
SOURCES.cc = $(addsuffix .cc,$(PARTS))
SOURCES.h = $(addsuffix .h,$(PARTS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%.o)

CCC = clang #g++
AR = ar
RANLIB = ranlib
RM = rm -f

LIBS=
#ORGCCFLAGS += -w -I.. -g -c
CCFLAGS = -ggdb -Wall -pedantic -w -c -std=c++11
#CCFLAGS = -I. -Wall -O3 -w -c -std=c++11 -pedantic -w -c 

LDFLAGS = -static -pthread
ARFLAGS = ruv

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

.PHONY: all

clean:
	$(RM) $(OBJECTS) $(LIB)

%.o: $(SOURCES)
	$(CCC) $(CCFLAGS) $(@:%.o=%.cc) -o $@

# building/linking library 
$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)


include ./makefile.inc
