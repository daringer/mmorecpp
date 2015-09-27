include ../Makefile.inc

TARGET = ../libtools.a

PARTS = template_parser config_manager xsqlite testing_suite xregex
SRC = $(addsuffix .cc, $(PARTS))
OBJS = $(addsuffix .o, $(PARTS))
INCS = $(addsuffix .h, $(PARTS))

$(TARGET): $(OBJS)

# objects
$(OBJS): $(SRC)
	$(CPP) -c $(CCFLAGS) $(@:%.o=%.cc) -o $(OBJ_TARGET_DIR)/$@
