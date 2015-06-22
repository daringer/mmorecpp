include ../Makefile.inc

TARGET = ../libtools.a

PARTS = exception xstring io xtime xunit converter
SRC = $(addsuffix .cc, $(PARTS))
OBJS = $(addsuffix .o, $(PARTS))
INCS = $(addsuffix .h, $(PARTS))

$(TARGET): $(OBJS)

# objects
$(OBJS): $(SRC)
	$(CPP) -c $(CCFLAGS) $(@:%.o=%.cc) -o $@
