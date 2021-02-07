clean:
	@rm -rf build

build-internal:
	echo OPTS ${CMAKE_OPTS_INTERNAL}
	@mkdir -p ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake ${CURDIR} $(CMAKE_OPTS_INTERNAL)
	@make -j12 -C ${BUILD_DIR} all

build-release:
	@mkdir -p build/release
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ${CURDIR}
	@make -j12 -C build/release sfe_basic sfe_preload sfe_preload_v2

run-internal:
	@make build-internal
	@${BUILD_DIR}/tests/test_libsfe_basic
	@if ! echo $$CXX | grep clang >/dev/null; then \
		LD_PRELOAD=${BUILD_DIR}/src/sfe/libsfe_preload.so ${BUILD_DIR}/tests/test_libsfe_preload; \
	fi
	@LD_PRELOAD=${BUILD_DIR}/src/sfe/libsfe_preload_v2.so ${BUILD_DIR}/tests/test_libsfe_preload

# export AVAILABLE_COMPILERS ?= g++-10
export AVAILABLE_COMPILERS ?= g++-10 clang++-10

# export CHECK_STANDARDS ?= c++17 # TODO

.PHONY: run-tests
run-tests:
	for CXX in ${AVAILABLE_COMPILERS}; do \
		make run-internal CMAKE_OPTS_INTERNAL="-DCMAKE_CXX_COMPILER=$$CXX" BUILD_DIR=build/test_$$CXX \
	; done

# && make run-internal CMAKE_OPTS_INTERNAL="-DCMAKE_CXX_COMPILER=${CXX} -DSANITIZE_ENABLE=ON" BUILD_DIR=build/test_$${CXX}_sanitizers \
