# live-transport-network-monitor


## Build

You will need:
 - CLang or other compiler
 - CMake
 - Conan >= 2.*
 - GNU Make

First run conan to generate and collect all dependencies:

```bash
make install
```

Then, you can generate the build and compile with CMake and Makefiles:

```bash
make build
```

Finally, you can run the executable as:
```
./build/network-monitor
```