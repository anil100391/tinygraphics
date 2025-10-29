# About
Tiny lib for setting up a graphics app using opengl and glfw

# Building

## Requirements

- CMake 3.10 or higher
- OpenGL, Glew and GLFW development libraries discoverable by CMake.
  These can be installed via system package manager on linux/macOS. e.g. on Ubuntu:

  ```bash
  sudo apt-get install libgl1-mesa-dev libglew-dev libglfw3-dev
  ```
  On windows we recommend using vcpkg to install these dependencies:
  ```powershell
  vcpkg install glew glfw
  ```

## Linux / macOS

```
git clone https:://github.com/anil100391/tinygraphics.git
cd tinygraphics
mkdir build
cmake -DWITH_EXAMPLES=1 -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build
```

## Windows (Using vcpkg)
```powershell
git clone https:://github.com/anil100391/tinygraphics.git
cd tinygraphics
mkdir build
cmake -DWITH_EXAMPLES=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -S . -B build
cmake --build build
```
If vcpkg is integrated in visual studio you can skip specifying CMAKE_TOOLCHAIN_FILE.
If you prefer building through visual studio directly just open local foler to project root
and let visual studio inbuild cmake support handle the build.
