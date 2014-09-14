
TARGET = libtools.a

CONTENTS = exception xstring xsocket fs template_parser config_manager xregex io testing_suite executor xlogger xtime threading mem_tracker xsqlite


CPP = clang++
AR = ar ruv
RANLIB = ranlib

SRCS = $(addsuffix .cc, $(CONTENTS))
INCS = $(addsuffix .h, $(CONTENTS))

OBJS = $(addsuffix .o, $(CONTENTS))

# cflags
COMMON_CCFLAGS = -c -I. -std=c++11 -pedantic -Wall
DEV_CCFLAGS = $(COMMON_CCFLAGS) -ggdb
FAST_CCFLAGS = $(COMMON_CCFLAGS) -O3
CCFLAGS = $(DEV_CCFLAGS)

# libs
LDFLAGS = -pthread -rdynamic


libtools.a: $(OBJS)
	$(AR) $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)

# objects
$(OBJS): $(SRCS)
	$(CPP) $(CCFLAGS) $(@:%.o=%.cc) -o $@

# generate cldoc reference documentation
cldoc:
	cldoc $(CCFLAGS) -- --output ../doc --report *.cc *.h

# format sourcecode with clang-format
format: 		
	clang-format -i -style=file *.cc *.h

# cleanup
clean:
	rm -f $(TARGET) $(OBJS)




