

all: src/libtools.a
	
check: src/libtools.a
	$(MAKE) -C tests tests

src/libtools.a:
	$(MAKE) -C src


clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
