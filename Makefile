GREEN=\e[32m
NORMAL=\e[0m

clean:
	@rm -rf build

build-release:
	@mkdir -p build/release
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ${CURDIR}
	@make -j12 -C build/release sfe_basic sfe_preload_v2

.ONESHELL:
run-internal:
	@set -e
	echo "${GREEN}Test with compiler '$$CXX'${NORMAL}"

	export CMAKE_OPTS_INTERNAL="-DCMAKE_CXX_COMPILER=$$CXX"

	export LD_PRELOAD_INTERNAL=$$BUILD_DIR/src/sfe/libsfe_preload_v2.so

	if [ "$$SANITIZE_ENABLE" = "ON" ]; then
		echo "${GREEN}And sanitizers${NORMAL}"
		export CMAKE_OPTS_INTERNAL="$$CMAKE_OPTS_INTERNAL -DSANITIZE_ENABLE=ON"
		export LD_PRELOAD_INTERNAL="$$($$CXX -print-file-name=libasan.so):$$LD_PRELOAD_INTERNAL"
	fi

	mkdir -p $$BUILD_DIR
	( cd $$BUILD_DIR && cmake ${CURDIR} $$CMAKE_OPTS_INTERNAL )
	make -j12 -C $$BUILD_DIR all

	set -x
	$$BUILD_DIR/tests/test_libsfe_basic
	LD_PRELOAD=$$LD_PRELOAD_INTERNAL $$BUILD_DIR/tests/test_libsfe_preload

AVAILABLE_COMPILERS ?= g++-10 clang++-10

.PHONY: run-tests
.ONESHELL:
run-tests:
	@set -e

	for CXX in ${AVAILABLE_COMPILERS}; do
		make run-internal BUILD_DIR=build/test_$${CXX}
		# if echo $$CXX | grep clang >/dev/null; then # leak with asan
		# 	continue
		# fi
		make run-internal BUILD_DIR=build/test_$${CXX}_sanitizers SANITIZE_ENABLE=ON
	done

	echo "${GREEN}Successfull!${NORMAL}"

