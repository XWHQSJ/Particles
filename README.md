# Particles

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=flat&logo=opengl&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

A collection of OpenGL particle system demos inspired by [NeHe's classic OpenGL tutorials](https://nehe.gamedev.net), with original additions and custom effects by XWHQSJ.

## About

This project started from NeHe's Lesson 19 (particle engine) and grew into a set of standalone particle effects, each exploring a different visual style. Every demo is a self-contained `.cpp` file that you can build and run independently.

## Demos

| File | Description |
|------|-------------|
| `particles.cpp` | Base particle system (from NeHe Lesson 19) |
| `butterfly.cpp` | Butterfly particle effect |
| `starCross.cpp` | Crossing-stars effect |
| `stars.cpp` | Stars particle system |
| `torchLight.cpp` | Torch / flame effect |
| `X-Men.cpp` | X-Men themed particle effect |

All source files are in the `allParticles/` directory.

## Screenshots

Screenshots of each effect are in the `pictures/` directory:

| Effect | Files |
|--------|-------|
| Particles | `particles.png` |
| Butterfly | `buftterfly.png`, `buftterfly (2).png` ... `buftterfly (6).png`, `butterfly.bmp` |
| Stars Cross | `starsCross.png`, `starsCross (2).png`, `starsCross (3).png` |
| Stars | `Stars.png`, `Stars (2).png`, `Stars (3).png` |
| Torch | `torch.png`, `torch (2).png` ... `torch (5).png` |
| X-Men | `X-Men.png`, `X-Men (2).png`, `X-Men (3).png` |

## Requirements

- **Visual Studio 2008 -- 2015** (or newer)
- **OpenGL libraries**: GLEW, GLUT (freeglut), and the standard OpenGL/GLU headers
- Windows SDK with `opengl32.lib` and `glu32.lib`

## Build

1. Open any `.cpp` file from `allParticles/` in Visual Studio (or add it to the existing `Lesson19.sln` solution).
2. Link the following libraries:

   ```
   opengl32.lib glu32.lib glew32.lib glut32.lib
   ```

3. For **VS2015 and newer**, add this pragma at the top of the source file:

   ```cpp
   #pragma comment(lib, "legacy_stdio_definitions.lib")
   ```

4. Build and run.

## VC++6.0 Compatibility Note

If you are using VC++6.0 instead of Visual Studio:

- Change `#include <GL/glew.h>` to `#include <GL/gl.h>`
- Change `#include <GL/glut.h>` to `#include <GL/glu.h>`
- Remove the `#pragma comment(lib, "legacy_stdio_definitions.lib")` line (it is only needed for VS2015+)

## References

- NeHe's OpenGL Tutorials: <https://nehe.gamedev.net>
- VS2015 OpenGL migration guide: <http://blog.csdn.net/wly95/article/details/51760082>

## Acknowledgments

Thanks to **NeHe** (Jeff Molofee) for the original particle system code and OpenGL tutorial series that inspired this project. NeHe's tutorials are freely available for educational use.

## License

[MIT](LICENSE)

Note: The original NeHe tutorial code has its own permissive terms. See [NeHe's readme](allParticles/NeHe's%20Readme.txt) for details.
