LIB = libtools.a

PARTS = config_manager exception xstring xsocket fs template_parser xregex io testing_suite executor xlogger xtime threading mem_tracker
#SOURCES.cc = $(wildcard *.cc) 
#SOURCES.h = $(wildcard *.h)
SOURCES.cc = $(addsuffix .cc, $(PARTS))
SOURCES.h = $(addsuffix .cc, $(PARTS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%.o)

CCC = clang++
AR = ar
RANLIB = ranlib
RM = rm -f

LIBS=

DEV_CCFLAGS = -ggdb -I. -Wall -pedantic -g -w -c -std=c++11 
FAST_CCFLAGS = -I. -Wall -O3 -w -c -std=c++11 -pedantic -w -c

CCFLAGS = $(DEV_CCFLAGS)

#LDFLAGS = -static -pthread
LDFLAGS = -pthread -rdynamic
ARFLAGS = ruv

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

.PHONY: all

clean:
	$(RM) $(OBJECTS) $(LIB)

%.o: $(SOURCES)
	$(CCC) $(CCFLAGS) -c -g $(@:%.o=%.cc) -o $@

# building/linking library 
$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)

cldoc:
	cldoc $(CCFLAGS) -- --output ../doc --report *.cc *.h
