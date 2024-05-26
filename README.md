# live-transport-network-monitor


## Build

You will need:
 - CLang or other compiler
 - CMake
 - Ninja
 - Conan >= 2.*

First run conan to generate and collect all dependencies:

```bash
cd live-transport-network-monitor
conan install . --output-folder=build --build=missing
```

Then, you can generate the build and compile with CMake and Ninja:

```bash
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
ninja
```

Finally, you can run the executable as:
```
./network-monitor
```