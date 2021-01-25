# run-with-ld-preload:
# 	mkdir -p build
# 	${CXX} -g -o build/libsfe.so src/sfe.cpp -ldl -shared -fPIC
# 	${CXX} -g src/test.cpp -ldl -o build/test
# 	LD_PRELOAD=./build/libsfe.so ./build/test
#
# run:
# 	mkdir -p build
# 	${CXX} -g -o build/libsfe.so src/sfe.cpp -ldl -shared -fPIC
# 	${CXX} -g src/test.cpp -ldl -lsfe -Lbuild -o build/test
# 	LD_LIBRARY_PATH=build ./build/test
# 	# LD_PRELOAD=./build/libsfe.so ./build/test

clean:
	@rm -rf build

.PHONY: build
build:
	@mkdir -p build
	@cd build && cmake ..
	@make -j 12 -C build all

.PHONY: test
test:
	@make build
	@build/tests/test
