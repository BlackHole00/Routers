# Routers
A dijkstra algorithm visualizer (a little bit broken)

## Building
### Prerequisites
This software currently builds only on windows using only the mingw compiler.

To get started install the following dependences:
 - [mingw-w64](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download)
 - [ninja build system](https://ninja-build.org/)
 - [meson build system](https://mesonbuild.com/)

If you are using [chocolatey](https://chocolatey.org/) you can download the dependences with the following commands:
 - `choco install mingw`
 - `choco install ninja`
 - `choco install meson`

### Building
Follow the following steps:
 - run the command `meson builddir` on the root of the project.
 - change directory inside the newly created `builddir` directory (`cd builddir`)
 - run `meson compile` to compile the project.
 - you will find the execuitable under the name of `router.exe`

## Notes:
 - It is theorically possible to build this code for linux or with the msvc compiler. You will need to update the raylib 
library providing you own prebuilt libraries. You may also need to upsate the corrisponding `meson.build` file.
 - The code is currently buggy and it is poorly structured. Do not use this code as an example!
