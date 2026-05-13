# About
Tiny lib for setting up a graphics app using opengl and glfw

# Building

## Requirements

- CMake 3.28 or higher
  ### Linux
- opengl libs and glfw dependencies
  e.g. on Ubuntu:

  ```bash
  sudo apt-get install libgl-dev libwayland-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev
  ```
  if using provided presets for configuring using cmake install ninja-build
  ```bash
  sudo apt-get install ninja-build
  ```
  ### Windows
  Visual Studio

## Building

In bash (on linux) or developer powershell (on windows)
* clone the repo
  ```
  git clone --recurse-submodules https:://github.com/anil100391/tinygraphics.git
  cd tinygraphics
  ```

* configure using debug or release preset
  ```
  cmake -B build -S . -G Ninja
  ```

* build
  ```
  cmake --build build
  ```

* install
  ```
  cmake --install build --prefix=./install # or your choice of install path
  ```
