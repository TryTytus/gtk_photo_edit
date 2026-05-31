<div align="center">

# GTK Photo Edit

A compact native desktop photo editor built with C++20, gtkmm 4, and libvips.

[![GitHub repository](https://img.shields.io/badge/GitHub-Repository-181717?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit)
[![Stars](https://img.shields.io/github/stars/TryTytus/gtk_photo_edit?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit/stargazers)
[![Forks](https://img.shields.io/github/forks/TryTytus/gtk_photo_edit?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit/forks)
[![Issues](https://img.shields.io/github/issues/TryTytus/gtk_photo_edit?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit/issues)
[![Last commit](https://img.shields.io/github/last-commit/TryTytus/gtk_photo_edit?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit/commits)
[![Repo size](https://img.shields.io/github/repo-size/TryTytus/gtk_photo_edit?style=flat-square&logo=github)](https://github.com/TryTytus/gtk_photo_edit)
![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=flat-square&logo=cmake&logoColor=white)
![GTK](https://img.shields.io/badge/GTK-4.0-4A86CF?style=flat-square&logo=gtk&logoColor=white)
![libvips](https://img.shields.io/badge/libvips-image_processing-7A52CC?style=flat-square)
![macOS](https://img.shields.io/badge/macOS-Homebrew_ready-000000?style=flat-square&logo=apple)

[Features](#features) | [Get Started](#get-started) | [Controls](#controls) | [Project Structure](#project-structure)

</div>

GTK Photo Edit is a small, fast image editing app with a native GTK4 interface.
Choose an image, adjust it with a focused slider panel, preview the result, and
save the edited output.

![Demo](assets/output.gif)



## Get Started

### Prerequisites

On macOS with Homebrew:

```sh
brew install gtkmm4 cmake vips pkg-config
```

You need CMake 3.20 or newer, a C++20 compiler, gtkmm 4, and libvips with C++
bindings available through `pkg-config`.

### Build and Run

```sh
cmake -S . -B build
cmake --build build
./build/gtk_second
```

## Controls

The editor currently exposes adjustments for contrast, sharpness, Sobel edges,
brightness, exposure, gamma, blur, saturation, warmth, tint, highlights, shadows,
black point, white point, vibrance, vignette, clarity, and noise smoothing.

