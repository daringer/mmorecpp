LIB = libtools.a

PARTS = config_manager exception xstring xsocket fs template_parser xregex io testing_suite executor xlogger xtime threading mem_tracker
SOURCES.cc = $(addsuffix .cc,$(PARTS))
SOURCES.h = $(addsuffix .h,$(PARTS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%.o)

CCC = g++
AR = ar
RANLIB = ranlib
RM = rm -f

LIBS=""
CCFLAGS = -ggdb -I. -I.. -Wall -pedantic -rdynamic -g -w -c -std=c++11
#CCFLAGS = -I. -Wall -O3 -w -c -std=c++11 -pedantic -w -c 

LDFLAGS = -static -pthread
ARFLAGS = ruv

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)

.PHONY: all clean

clean:
	$(RM) $(OBJECTS) $(LIB)

%.o: $(SOURCES)
	$(CCC) $(CCFLAGS) -c -g $(@:%.o=%.cc) -o $@

#$(PROGRAM): $(SOURCES) $(OBJECTS) 
#	$(CCC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

# building/linking library 
$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)
