clean:
	@rm -rf build

build-release:
	# TODO

build-internal:
	@mkdir -p ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake ${CURDIR} $(CMAKE_OPTS_INTERNAL)
	@make -j12 -C ${BUILD_DIR} all

test-one = CMAKE_OPTS_INTERNAL=$2 BUILD_DIR=build/$1 make build-internal && build/$1/tests/test

# export AVAILABLE_COMPILERS ?= g++
export AVAILABLE_COMPILERS ?= g++-10 clang++-10
# export CHECK_STANDARDS ?= c++17 # TODO

.PHONY: test
test:

	$(foreach CXX, ${AVAILABLE_COMPILERS}, \
		$(call test-one,test_${CXX},"-DCMAKE_CXX_COMPILER=${CXX}") && \
		$(call test-one,test_sanitizers_${CXX},"-DSANITIZE_ENABLE=ON -DCMAKE_CXX_COMPILER=${CXX}"); \
	)

# @$(call test-one,test)
# @$(call test-one,test_sanitizers,-DSANITIZE_ENABLE=ON)
