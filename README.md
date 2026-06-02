# About
Tiny lib for setting up a graphics app using opengl + glfw + dearimgui

# Building

## Requirements

- CMake 3.28 or higher
  ### Linux
- opengl libs and glfw dependencies
  e.g. on Ubuntu:

  ```bash
  sudo apt-get install libgl-dev libwayland-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev
  ```
  optionally ninja-build for faster builds
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

* configure (skip -G Ninja if you don't have ninja installed)
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
