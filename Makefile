

all: src/libtools.a
	
check: src/libtools.a
	$(MAKE) -C tests tests

src/libtools.a:
	$(MAKE) -C src


clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean

# format sourcecode with clang-format
format:
	$(MAKE) -C src format
	$(MAKE) -C tests format
