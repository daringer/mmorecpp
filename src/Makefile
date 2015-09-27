include Makefile.inc

TOPTARGET=libtools.a

PARTS=core/ platform/ experimental/ ooapi/ math/

CORE_A=core/libtools_core.a
PLATFORM_A=platform/libtools_platform.a
EXPERIM_A=experimental/libtools_experimental.a
OOAPI_A=ooapi/libtools_ooapi.a
MATH_A=math/libtools_math.a

CORE_SRC=converter.cc xstring.cc xunit.cc xtime.cc exception.cc io.cc
PLAT_SRC=xlogger.cc executor.cc fs.cc threading.cc xsocket.cc 
EXPERIM_SRC=xncurses.cc

# additional .h in ooapi -> circular_buffer.h
OOAPI_SRC=config_manager.cc template_parser.cc testing_suite.cc \
	  xregex.cc xsqlite.cc

# only .h inside math atm (basic.h & moving_ana.h)
MATH_SRC=

PARTS_A=$(CORE_A) $(PLATFORM_A) $(OOAPI_A) $(MATH_A) #$(EXPERIM_A)

OBJ_FILES=$(shell find . -name "*.o" | xargs)

$(TOPTARGET): MYOBJDIR=core/$(OBJ_TARGET_DIR)
$(TOPTARGET): $(PARTS_A)
	$(AR) $(TOPTARGET) $(OBJ_FILES)
	$(RANLIB) $(TOPTARGET)

$(PLATFORM_A): MYPARTS=$(subst .cc,,$(PLAT_SRC))
$(PLATFORM_A): $(addprefix platform/, $(PLAT_SRC))
	mkdir -p  $(MYOBJDIR)
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)' PARTS='$(MYPARTS)'

$(CORE_A): MYPARTS=$(subst .cc,,$(CORE_SRC))
$(CORE_A): $(addprefix core/, $(CORE_SRC))
	mkdir -p  $(MYOBJDIR)
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)' PARTS='$(MYPARTS)'

$(OOAPI_A): MYPARTS=$(subst .cc,,$(OOAPI_SRC))
$(OOAPI_A): $(addprefix ooapi/, $(OOAPI_SRC))
	mkdir -p  $(MYOBJDIR)
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)' PARTS='$(MYPARTS)'

$(MATH_A): MYPARTS=$(subst .cc,,$(MATH_SRC))
$(MATH_A): $(addprefix math/, $(MATH_SRC))
	mkdir -p  $(MYOBJDIR)
	+make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)' PARTS='$(MYPARTS)'

$(EXPERIM_A): MYPARTS=$(subst .cc,,$(EXPERIM_SRC))
$(EXPERIM_A): $(addprefix experimental/, $(EXPERIM_SRC))
	mkdir -p  $(MYOBJDIR)
	make -C $(shell dirname $@) CCFLAGS='$(CCFLAGS)' PARTS='$(MYPARTS)'

# build with optimization CCFLAGS 
optimize: CCFLAGS=$(FAST_CCFLAGS)
optimize: $(TOPTARGET)

# generate cldoc reference documentation
cldoc:
	cldoc $(CCFLAGS) -- --output ../doc --report */*.cc */*.h

# format sourcecode with clang-format
format: 		
	clang-format -i -style=file */*.cc */*.h *.h

# cleanup
clean:
	find . -name "*.a" -or -name "*.o" -or -name "*.so" | xargs rm -f

# install .so file to /usr/lib
install:
	install -m 664 -o root -g root $(NAME) /usr/lib 
	ln -s /usr/lib/$(NAME) /usr/lib/$(LIBNAME)


# make shared lib (.so)
shared: OLDCCFLAGS = $(CCFLAGS)
shared: CCFLAGS = $(SHARED_CCFLAGS)	
shared: $(PARTS_A)
	$(CPP) $(OLDCCFLAGS) -shared -Wl,-soname,$(LIBNAME) -o $(NAME) $(OBJ_FILES) $(LIBS)

