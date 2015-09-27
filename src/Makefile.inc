
NAME=libmmorecpp.so
LIBNAME=$(NAME).1.0.0.so

# bins
CPP=clang++
AR=ar rv
RANLIB=ranlib
RM=rm -f

# cflags
COMMON_CCFLAGS = -I. -std=c++11 -pedantic -Wall
DEV_CCFLAGS = $(COMMON_CCFLAGS) -ggdb

OPT_CCFLAGS = $(COMMON_CCFLAGS) -O3
FAST_CCFLAGS = $(OPT_CCFLAGS) -march=corei7-avx \
	-fstack-protector --param=ssp-buffer-size=4 \
	-D_FORTIFY_SOURCE=2

CCFLAGS ?= $(DEV_CCFLAGS)
#CCFLAGS = $(FAST_CCFLAGS)
SHARED_CCFLAGS = -fPIC $(DEV_CCFLAGS)

# libs
LDLIBS = -pthread -lncurses -lc
LDFLAGS = $(LIBS) -rdynamic

