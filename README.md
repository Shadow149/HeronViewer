<div align="center" id="user-content-toc" style="padding: 100px 0;">
  <ul><summary>
    <img src="./HeronViewer/icon/heron_48.png"/>&nbsp;
    <h1>Heron</h1><br>
  </summary></ul>
</div>
<p align="center">
  <img alt="HeronViewer preview" src="./docs/preview.png" width="450" />
</p>
<h3 align="center" style="font-size: 10px;">Heron Edits Raw with Opengl Natively</h3>

<p align="center">
 <img alt="Release" src="https://img.shields.io/badge/Release-v0.1.0.1-2CAD31.svg?style=for-the-badge"/>
 <img src="https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white">
 <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">

</p>

## Contents

- [Contents](#contents)
- [About](#about)
- [Features](#features)
- [Usage](#usage)
  - [Keyboard shortcuts](#keyboard-shortcuts)
- [Download](#download)
- [Building from source](#building-from-source)
  - [Requirements](#requirements)
  - [Windows](#windows)
  - [Linux](#linux)
- [Documentation](#documentation)
- [Contributing](#contributing)

## About

Heron is an open source, free to use, raw image editor prioritising performance and ease of use, written in C++.

## Features

- Tone-curve based editing
- Intuitive colour wheels for advanced colour grading
- OpenGL shader based editing for ultra-fast performance
- Histogram, vectorscope and waveform graphics
- Automatic settings saving
- Local catalog to store edited photos

## Usage

### Keyboard shortcuts

| Key | Action |
| - | - |
| B | Show image before edits |
| ALT | Highlight image peaking/crush |
| SPACE | Toggle black/white background |
| SHIFT+SCROLL | Horizontal image pan |
| SHIFT+Hue select | Precision movement |

## Download

Binaries coming soon...

## Building from source

### Requirements

- OpenGL 4.3
- **Windows**
    - Visual Studio 20xx
- **Linux**
    - G++ ...
    - CMake 3.0.0+

### Windows

Open & run visual studio solution

### Linux

Update repos - `apt-get update`

Cmake 3+ - `apt install cmake pkg-config`

OpenGL 4+ - `apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev`

GLFW 3 - `apt-get install libglew-dev libglfw3-dev libglm-dev`

Libraw - see installation [docs](https://www.libraw.org/docs/Install-LibRaw-eng.html). The library must be installed locally (within `/usr/local/...`) for compilation to work.

To build from root dir, run:
```bash
cmake --build build
```

To run (MUST BE FROM BUILD DIR):
```bash
./Heron
```

## Documentation

## Contributing
