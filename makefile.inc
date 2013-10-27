analyze:
	scan-build -V -o /tmp/clang_sca make -j4 CCC="scan-build -o /tmp/clang_sca/ $(CCC)"

.PHONY: analyze
