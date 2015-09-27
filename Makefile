all: src/libtools.a
	
test: src/libtools.a 
	$(MAKE) -C tests tests

# build with full optimization
optimize:
	$(MAKE) -C src optimize
	$(MAKE) -C tests optimize
	$(MAKE) -C tests tests

src/libtools.a:
	$(MAKE) -C src


clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean


format:
	$(MAKE) -C src format
	$(MAKE) -C tests format
