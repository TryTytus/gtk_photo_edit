# gtk_second

A minimal object-oriented C++ gtkmm 4 app that opens a window and shows
`Hello, world!`.

## Prerequisites

On macOS with Homebrew:

```sh
brew install gtkmm4 cmake vips
```

## Build and Run

```sh
cmake -S . -B build
cmake --build build
./build/gtk_second
```

If your shell uses a non-Homebrew `pkg-config`, this project still checks the
standard Homebrew paths from CMake. You can also run manually with:

```sh
PKG_CONFIG_PATH=/opt/homebrew/lib/pkgconfig:/opt/homebrew/share/pkgconfig cmake -S . -B build
```

If macOS prints a `C.UTF-8` locale warning when launching from an automated
shell, use an installed UTF-8 locale:

```sh
env LANG=pl_PL.UTF-8 LC_ALL=pl_PL.UTF-8 ./build/gtk_second
```
