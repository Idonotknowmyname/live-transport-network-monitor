BUILD_TYPE ?= Release

.PHONY: all install build

all: install build
	@

install:
	@ conan install . --build=missing -sbuild_type=${BUILD_TYPE}

build:
	@ mkdir -p build
	@ cd build && cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=${BUILD_TYPE}/generators/conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
	@ cd build && make

clean:
	@ find build ! -name .gitignore -depth 1 | xargs rm -r
	@ rm -f CMakeUserPresets.json

clean-install:
	@ conan clean cache  "*" -c
