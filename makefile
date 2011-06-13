include $(AMV_HOME)/templates/makefile.libs
include $(AMV_HOME)/templates/makefile$(EXTENSION_LONG)

LIB = libtools$(EXTENSION).a

TOOLS = exception xstring xsocket fs template_parser xregex io testing_suite config_manager #threading
SOURCES.cc = $(addsuffix .cc,$(TOOLS))
SOURCES.h = $(addsuffix .h,$(TOOLS))

SOURCES = $(SOURCES.h)	$(SOURCES.cc)
OBJECTS = $(SOURCES.cc:%.cc=%$(EXTENSION).o)

ORGCCFLAGS += -w -I.. -g
CCFLAGS = -ggdb -I. -I.. -I$(ANTLR_H) -I$(TOOLS_DIR) -DLINUX -Wall 

all:	 $(LIB)
objects: $(SOURCES) $(OBJECTS)
sources: $(SOURCES)
targets: $(SOURCES)

clean:
	# Removing libs from $(LAPACKDIR_OHNE_PERF)!!!
	$(RM) $(OBJECTS) core $(LIB)

.PARALLEL: $(OBJECTS)


# building object files here
%$(EXTENSION).o: %.cc $(SOURCES.h) $(MAKEFILE) $(MAKEFILEINC)
	@rm -f $@
	$(CCC) $(CCFLAGS) -c -g $(@:%$(EXTENSION).o=%.cc) -o $@
	@chmod g+w,a+r $@

# building/linking library 
$(LIB): $(SOURCES) $(OBJECTS) $(MAKEFILE) $(MAKEFILEINC)
	# Compiling $(LIB)
	@rm -f $(LIB)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)

incl_file: $(INCL_LIB)
