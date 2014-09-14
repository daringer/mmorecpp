LIB = libtools.a

PARTS = config_manager exception xstring xsocket fs template_parser xregex io testing_suite executor xlogger xtime threading mem_tracker xsqlite

SOURCES.cc = $(addsuffix .cc, $(PARTS))
SOURCES.h = $(addsuffix .h, $(PARTS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%.o)

CCC = clang++
AR = ar
RANLIB = ranlib
RM = rm -f

LIBS=

COMMON_CCFLAGS = -c -I. -std=c++11 -pedantic -Wall
DEV_CCFLAGS = $(COMMON_CCFLAGS) -ggdb
FAST_CCFLAGS = $(COMMON_CCFLAGS) -O3 

CCFLAGS = $(DEV_CCFLAGS)

LDFLAGS = -pthread -rdynamic
ARFLAGS = ruv

.PHONY: all clean

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	$(RM) $(OBJECTS) $(LIB)

%.o: $(SOURCES)
	$(CCC) $(CCFLAGS) $(@:%.o=%.cc) -o $@

# building/linking library 
$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)

# generate cldoc reference documentation
cldoc:
	cldoc $(CCFLAGS) -- --output ../doc --report *.cc *.h

# format sourcecode with clang-format
format:
	clang-format -i -style=file *.cc *.h
