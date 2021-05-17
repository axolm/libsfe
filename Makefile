PREFIX ?= /usr/local
LIBDIR = $(PREFIX)/lib
SFE_HEADERS_DIR = $(PREFIX)/include/sfe

clean:
	@rm -rf build

# Build release versions of libraries
.ONESHELL:
build-release:
	@set -e

	mkdir -p build/release && cd build/release
	cmake -DCMAKE_BUILD_TYPE=Release ${CURDIR}
	make -j16 sfe_basic sfe_preload

# Install headers and libraries (Tested only for Linux)
.ONESHELL:
install:
	@set -e

	mkdir -p ${SFE_HEADERS_DIR}
	echo "Install headers to ${SFE_HEADERS_DIR}"
	install -m644 include/sfe/sfe.hpp ${SFE_HEADERS_DIR}
	install -m644 include/sfe/stacktrace.hpp ${SFE_HEADERS_DIR}
	echo "Install libs to ${LIBDIR}"
	install -m644 build/release/src/sfe/libsfe_basic.so ${LIBDIR}
	install -m644 build/release/src/sfe/libsfe_preload.so ${LIBDIR}
	echo 'Updating linker cache'
	ldconfig
	echo "Install finished!"


.ONESHELL:
run-tests-internal:
	@set -e

	echo "Test with compiler '$$CXX'"

	CMAKE_OPTS_INTERNAL="-DCMAKE_CXX_COMPILER=$$CXX"

	LD_PRELOAD_INTERNAL=$$BUILD_DIR/src/sfe/libsfe_preload.so
	if [ "$$SANITIZE_ENABLE" = "ON" ]; then
		echo "And sanitizers"
		CMAKE_OPTS_INTERNAL="$$CMAKE_OPTS_INTERNAL -DSANITIZE_ENABLE=ON"
		LD_PRELOAD_INTERNAL="$$($$CXX -print-file-name=libasan.so):$$LD_PRELOAD_INTERNAL"
	fi

	mkdir -p $$BUILD_DIR
	( cd $$BUILD_DIR && cmake ${CURDIR} $$CMAKE_OPTS_INTERNAL )
	make -j16 -C $$BUILD_DIR all

	set -x
	$$BUILD_DIR/tests/test_libsfe_basic
	LD_PRELOAD=$$LD_PRELOAD_INTERNAL $$BUILD_DIR/tests/test_libsfe_preload
	LD_PRELOAD=$$LD_PRELOAD_INTERNAL $$BUILD_DIR/tests/test_terminate_preload


# `AVAILABLE_COMPILERS=g++ make run-tests` -- Run tests with chosen compilers
AVAILABLE_COMPILERS ?= g++ clang++
.PHONY: run-tests
.ONESHELL:
run-tests:
	@set -e

	for CXX in ${AVAILABLE_COMPILERS}; do
		make run-tests-internal BUILD_DIR=build/test_$${CXX}
		make run-tests-internal BUILD_DIR=build/test_$${CXX}_sanitizers SANITIZE_ENABLE=ON
	done

	echo "Successfull!"

.PHONY: run-benchmarks
.ONESHELL:
run-benchmarks:
	@set -e
	BUILD_DIR=build/bench
	LD_PRELOAD_INTERNAL=$$BUILD_DIR/src/sfe/libsfe_preload.so
	mkdir -p $$BUILD_DIR
	( cd $$BUILD_DIR && cmake -DCMAKE_BUILD_TYPE=Release ${CURDIR} )
	make -j16 -C $$BUILD_DIR bench_libsfe sfe_preload
	$$BUILD_DIR/benchmarks/bench_libsfe
	LD_PRELOAD=$$LD_PRELOAD_INTERNAL $$BUILD_DIR/benchmarks/bench_libsfe
	echo "Successfull!"
