<p align="center">
  <img alt="HeronViewer preview" src="./docs/preview.png" width="450" />
  <h1 align="center">Heron<br>
  <font size="4">Heron Edits Raw with Opengl Natively</font>
  </h1>
</p>

<p align="center">
 <img alt="Release" src="https://img.shields.io/badge/Release-v0.1.0.1-2CAD31.svg?style=for-the-badge"/>
 <img src="https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white">
 <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">

</p>

## Contents

- [About](#about)
- [Features](#features)
- [Usage](#usage)
- [Download](#download)
- [Building from source](#building-from-source)
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

Freeimage - `apt-get install libfreeimage3 libfreeimage-dev`

To build, run:
```bash
cmake; make
```

To run:
```bash
./heron
```

## Documentation

## Contributing