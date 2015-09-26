include ../Makefile.inc

TARGET = ../libtools.a

PARTS = funcs
SRC = $(addsuffix .cc, $(PARTS))
OBJS = $(addsuffix .o, $(PARTS))
INCS = $(addsuffix .h, $(PARTS))

$(TARGET): $(OBJS)

# objects
$(OBJS): $(SRC)
	$(CPP) -c $(CCFLAGS) $(@:%.o=%.cc) -o $@
