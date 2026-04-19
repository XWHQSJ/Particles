# Particles

[![CI](https://github.com/XWHQSJ/Particles/actions/workflows/ci.yml/badge.svg)](https://github.com/XWHQSJ/Particles/actions/workflows/ci.yml)
[![Release](https://github.com/XWHQSJ/Particles/actions/workflows/release.yml/badge.svg)](https://github.com/XWHQSJ/Particles/actions/workflows/release.yml)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?logo=opengl&logoColor=white)
![WebGL](https://img.shields.io/badge/WebGL-990000?logo=webgl&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A collection of OpenGL particle system demos inspired by [NeHe's classic OpenGL tutorials](https://nehe.gamedev.net), with original additions and custom effects by XWHQSJ.

## Try in Browser

All 11 demos run in the browser via Emscripten/WebAssembly (WebGL 2).

**Live demos**: [xwhqsj.github.io/Particles](https://xwhqsj.github.io/Particles/) (deployed via gh-pages CI on push to master)

| Demo | Description |
|------|-------------|
| particles | Base NeHe-style particle system |
| butterfly | Symmetric paired systems with diagonal gravity |
| starCross | Two perpendicular crossing streams |
| stars | 7 preset particle systems at different positions |
| torchLight | Flame / torch upward effect |
| X-Men | Diagonal burst particle arrays |
| fire | Upward flame with orange-red palette, rapid fade |
| fountain | Circular spawn via sin/cos, high initial Y, strong gravity |
| snow | Gentle snowfall with drift, ground collision |
| galaxy | Parametric spiral arms with tangential velocity |
| explosion | Radial burst, zero gravity, rapid fade |

## Gallery

### Original Demos (NeHe-derived)

<table>
<tr>
  <td align="center"><img src="pictures/particles.png" width="240"/><br><b>Particles</b></td>
  <td align="center"><img src="pictures/buftterfly.png" width="240"/><br><b>Butterfly</b></td>
  <td align="center"><img src="pictures/starsCross.png" width="240"/><br><b>StarCross</b></td>
</tr>
<tr>
  <td align="center"><img src="pictures/Stars.png" width="240"/><br><b>Stars</b></td>
  <td align="center"><img src="pictures/torch.png" width="240"/><br><b>TorchLight</b></td>
  <td align="center"><img src="pictures/X-Men.png" width="240"/><br><b>X-Men</b></td>
</tr>
</table>

### New Demos (added in v2)

<table>
<tr>
  <td align="center"><img src="docs/images/fire.png" width="240"/><br><b>Fire</b></td>
  <td align="center"><img src="docs/images/fountain.png" width="240"/><br><b>Fountain</b></td>
  <td align="center"><img src="docs/images/snow.png" width="240"/><br><b>Snow</b></td>
</tr>
<tr>
  <td align="center"><img src="docs/images/galaxy.png" width="240"/><br><b>Galaxy</b></td>
  <td align="center"><img src="docs/images/explosion.png" width="240"/><br><b>Explosion</b></td>
  <td></td>
</tr>
</table>

*New demo snapshots rendered by `qa/render_demo_snapshots.cpp` -- a standalone software particle renderer (no GL context needed).*

## Project Structure

```
Particles/
├── allParticles/       # Original Win32 + legacy GL demos (historical reference)
├── common/             # Cross-platform core library (GLFW + OpenGL)
│   ├── app.h/cpp       # GLFW window, GL context, main loop, CLI parsing
│   ├── particle.h/cpp  # ParticleSystem engine (init, update, render)
│   ├── texture.h/cpp   # Image loading via stb_image
│   ├── color.h         # Color palette definitions
│   └── renderer_modern.h/cpp  # OpenGL 3.3 / WebGL 2 renderer (VBO + shaders)
├── demos/              # 11 runnable cross-platform demos
├── tests/              # GoogleTest physics unit tests (12 tests)
├── shaders/            # GLSL vertex + fragment shaders
├── pictures/           # Screenshots of original effects
├── .github/workflows/  # CI: sanitizers, gh-pages, release, CodeQL
└── CMakeLists.txt      # Top-level build (FetchContent for GLFW + GoogleTest)
```

### Directory Notes

- **`demos/`** — Modern cross-platform demos (GLFW + OpenGL). Each demo is 30-55 lines of configuration code.
- **`allParticles/`** — Historical legacy Win32 demos. Kept as reference; requires VS 2008-2015 on Windows.
- **`web/`** — Browser builds via Emscripten. Generated automatically by the gh-pages CI workflow.

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

See the [Gallery](#gallery) above for visual previews of all 11 demos.

## Build

### Native (Desktop)

Requires CMake 3.16+ and a C++17 compiler. No external dependencies needed -- GLFW and GoogleTest are fetched automatically.

```bash
cmake -B build -S . -DUSE_MODERN_GL=ON
cmake --build build -j
```

### WebAssembly (Emscripten)

Requires [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

```bash
emcmake cmake -B build-web -S . -DUSE_MODERN_GL=ON
emmake make -C build-web -j
# Output: build-web/demos/{name}.html + .js + .wasm
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_MODERN_GL` | `ON` | Compile modern OpenGL 3.3 / WebGL 2 renderer |
| `BUILD_TESTS` | `ON` | Build physics unit tests (auto-disabled for Emscripten) |

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
- **Modern renderer**: OpenGL 3.3 core profile / WebGL 2 with VBOs, GLSL point sprites, additive blending
- **Emscripten support**: `emscripten_set_main_loop_arg()` replaces the blocking `while` loop; legacy renderer transparently delegates to the modern renderer under WebGL

## Platform Support

| Platform | Status |
|----------|--------|
| macOS | Tested (GLFW + OpenGL) |
| Linux | Tested in CI (GLFW + OpenGL) |
| Windows | Should work (GLFW + OpenGL, MSVC or MinGW) |
| Browser | WebAssembly via Emscripten (WebGL 2) |

The original Win32 demos in `allParticles/` require Visual Studio 2008-2015 on Windows.

## CI/CD

| Workflow | Trigger | Description |
|----------|---------|-------------|
| [CI](https://github.com/XWHQSJ/Particles/actions/workflows/ci.yml) | Push/PR to master | ASan + UBSan matrix on Ubuntu + macOS |
| [gh-pages](https://github.com/XWHQSJ/Particles/actions/workflows/gh-pages.yml) | Push to master | Build WASM demos, deploy to GitHub Pages |
| [Release](https://github.com/XWHQSJ/Particles/actions/workflows/release.yml) | Tag `v*` | Prebuilt binaries for Linux + macOS |
| [CodeQL](https://github.com/XWHQSJ/Particles/actions/workflows/codeql.yml) | Push/PR + weekly | Static analysis |

## Acknowledgments

Thanks to **NeHe** (Jeff Molofee) for the original particle system code and OpenGL tutorial series that inspired this project. NeHe's tutorials are freely available for educational use. See [NeHe's readme](allParticles/NeHe's%20Readme.txt) for details.

## References

- NeHe's OpenGL Tutorials: <https://nehe.gamedev.net>

## License

[MIT](LICENSE)
