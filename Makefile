.PHONY: docker-image config build run tests sysroot clean

docker-image:
	docker build -t patcher-mingw64 .

config:
	docker run --rm -u $(shell id -u):$(shell id -g) -v "$(PWD)":"$(PWD)" -w "$(PWD)" patcher-mingw64 cmake -B build -DCMAKE_TOOLCHAIN_FILE=mingw_toolchain.cmake -G "Ninja Multi-Config"

build:
	docker run --rm -u $(shell id -u):$(shell id -g) -v "$(PWD)":"$(PWD)" -w "$(PWD)" patcher-mingw64 cmake --build build --config Debug

run: build
	./build/src/Debug/project.exe

tests: build
	./build/tests/Debug/unit_tests.exe --gtest_color=yes

pack: build
	docker run --rm -u $(shell id -u):$(shell id -g) -v "$(PWD)":"$(PWD)" -w "$(PWD)/build" patcher-mingw64 cpack . -C Debug

sysroot:
	docker run --rm -v "$(PWD)/sysroot":/out patcher-mingw64 bash -c "cp -r /usr/local/i686-w64-mingw32/include /out/ && cp -r /usr/local/i686-w64-mingw32/include/c++ /out/ || true"

release:
	docker run --rm -u $(shell id -u):$(shell id -g) -v "$(PWD)":"$(PWD)" -w "$(PWD)" patcher-mingw64 cmake --build build --config Release
	docker run --rm -u $(shell id -u):$(shell id -g) -v "$(PWD)":"$(PWD)" -w "$(PWD)/build" patcher-mingw64 cpack . -C Release

clean:
	rm -rf build
