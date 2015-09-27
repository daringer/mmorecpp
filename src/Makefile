include Makefile.inc

TOPTARGET=libtools.a

PARTS=core/ platform/ experimental/ ooapi/ math/

CORE_A=core/libtools_core.a
PLATFORM_A=platform/libtools_platform.a
EXPERIM_A=experimental/libtools_experimental.a
OOAPI_A=ooapi/libtools_ooapi.a
MATH_A=math/libtools_math.a

CORE_SRC=converter.cc xstring.cc xunit.cc xtime.cc exception.cc
PLAT_SRC=xlogger.cc executor.cc fs.cc threading.cc xsocket.cc 
EXPERIM_SRC=xncurses.cc
OOAPI_SRC=config_manager.cc template_parser.cc testing_suite.cc \
	  xregex.cc xsqlite.cc
MATH_SRC=funcs.cc

PARTS_A=$(CORE_A) $(PLATFORM_A) $(OOAPI_A) $(MATH_A) #$(EXPERIM_A)

OBJ_FILES=$(shell find . -name "*.o" | xargs)

$(TOPTARGET): $(PARTS_A)
	$(AR) $(TOPTARGET) $(OBJ_FILES) #$(shell find . -name "*.o" | xargs)
	$(RANLIB) $(TOPTARGET)

$(PLATFORM_A): $(addprefix platform/, $(PLAT_SRC))
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)'

$(CORE_A): $(addprefix core/, $(CORE_SRC))
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)'

$(OOAPI_A): $(addprefix ooapi/, $(OOAPI_SRC))
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)'

$(MATH_A): $(addprefix math/, $(MATH_SRC))
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)'

#$(EXPERIM_A): $(addprefix experimental/, $(EXPERIM_SRC))
#	make -C $(shell dirname $@) CCFLAGS=$(CCFLAGS)

# generate cldoc reference documentation
cldoc:
	cldoc $(CCFLAGS) -- --output ../doc --report */*.cc */*.h

# format sourcecode with clang-format
format: 		
	clang-format -i -style=file */*.cc */*.h *.h

# cleanup
clean:
	find . -name "*.a" -or -name "*.o" | xargs rm -f

# make shared lib (.so)
shared: OLDCCFLAGS = $(CCFLAGS)
shared: CCFLAGS = $(SHARED_CCFLAGS)	
shared: $(PARTS_A)
	$(CPP) $(OLDCCFLAGS) -shared -Wl,-soname,$(LIBNAME) -o $(NAME) $(OBJ_FILES) $(LIBS)

