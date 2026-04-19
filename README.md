# Particles

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=flat&logo=opengl&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat&logo=cmake&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

A collection of OpenGL particle system demos inspired by [NeHe's classic OpenGL tutorials](https://nehe.gamedev.net), with original additions and custom effects by XWHQSJ.

## Project Structure

```
Particles/
├── allParticles/       # Original Win32 + legacy GL demos (historical reference)
├── common/             # Cross-platform core library (GLFW + OpenGL)
│   ├── app.h/cpp       # GLFW window, GL context, main loop, CLI parsing
│   ├── particle.h/cpp  # ParticleSystem engine (init, update, render)
│   ├── texture.h/cpp   # Image loading via stb_image
│   ├── color.h         # Color palette definitions
│   └── renderer_modern.h/cpp  # OpenGL 3.3 core renderer (VBO + shaders)
├── demos/              # 11 runnable cross-platform demos
├── tests/              # GoogleTest physics unit tests
├── shaders/            # GLSL vertex + fragment shaders
├── pictures/           # Screenshots of original effects
└── CMakeLists.txt      # Top-level build (FetchContent for GLFW + GoogleTest)
```

## Demos

### Ported from Original (6)

| Demo | Description | Original |
|------|-------------|----------|
| `particles` | Base NeHe-style particle system | `allParticles/particles.cpp` |
| `butterfly` | Symmetric paired systems with diagonal gravity | `allParticles/butterfly.cpp` |
| `starCross` | Two perpendicular crossing streams | `allParticles/starCross.cpp` |
| `stars` | 7 preset particle systems at different positions | `allParticles/stars.cpp` |
| `torchLight` | Flame/torch upward effect | `allParticles/torchLight.cpp` |
| `X-Men` | Diagonal burst particle arrays | `allParticles/X-Men.cpp` |

### New Demos (5)

| Demo | Description |
|------|-------------|
| `fire` | Upward flame with orange-red palette, rapid fade |
| `fountain` | Circular spawn via sin/cos, high initial Y, strong gravity |
| `snow` | Gentle snowfall with drift, ground collision |
| `galaxy` | Parametric spiral arms with tangential velocity |
| `explosion` | Radial burst, zero gravity, rapid fade |

## Screenshots

Screenshots of the original effects are in `pictures/`:

| Effect | Files |
|--------|-------|
| Particles | `particles.png` |
| Butterfly | `buftterfly.png` ... `buftterfly (6).png` |
| Stars Cross | `starsCross.png` ... `starsCross (3).png` |
| Stars | `Stars.png` ... `Stars (3).png` |
| Torch | `torch.png` ... `torch (5).png` |
| X-Men | `X-Men.png` ... `X-Men (3).png` |

## Build

Requires CMake 3.16+ and a C++17 compiler. No external dependencies needed -- GLFW and GoogleTest are fetched automatically.

```bash
cmake -B build -S . -DUSE_MODERN_GL=ON
cmake --build build -j
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_MODERN_GL` | `ON` | Compile modern OpenGL 3.3 renderer |
| `BUILD_TESTS` | `ON` | Build physics unit tests |

### Run Tests

```bash
ctest --test-dir build --output-on-failure
```

### Run a Demo

```bash
cd build/demos
# Run from project root so textures can be found:
cd ../.. && ./build/demos/particles
```

### CLI Options (all demos)

```
--particles N       Number of particles (default: demo-specific)
--gravity x,y,z     Gravity vector (e.g. 0,-0.8,0)
--slowdown F        Slowdown factor (default: 2.0)
--help, -h          Show help
```

### Controls

| Key | Action |
|-----|--------|
| ESC | Quit |
| SPACE | Burst (reset all particles) |

## Architecture

The `common/` library eliminates ~2500 lines of duplicated Win32+GL boilerplate from the originals. Each demo is now 30-55 lines of configuration code.

- **ParticleSystem**: configurable count, gravity, velocity spread, fade, palette, floor collision, auto-respawn
- **Legacy renderer**: OpenGL 2.1 immediate mode (`glBegin`/`glEnd` triangle strips)
- **Modern renderer**: OpenGL 3.3 core profile with VBOs, GLSL point sprites, additive blending

## Platform Support

| Platform | Status |
|----------|--------|
| macOS | Tested (GLFW + OpenGL) |
| Linux | Should work (GLFW + OpenGL) |
| Windows | Should work (GLFW + OpenGL, MSVC or MinGW) |

The original Win32 demos in `allParticles/` require Visual Studio 2008-2015 on Windows.

## Acknowledgments

Thanks to **NeHe** (Jeff Molofee) for the original particle system code and OpenGL tutorial series that inspired this project. NeHe's tutorials are freely available for educational use. See [NeHe's readme](allParticles/NeHe's%20Readme.txt) for details.

## References

- NeHe's OpenGL Tutorials: <https://nehe.gamedev.net>

## License

[MIT](LICENSE)
