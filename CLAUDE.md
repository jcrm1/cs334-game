# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from repo root)
cmake -B build

# Build
cmake --build build

# Run (from repo root — shaders/resources use relative paths)
./build/Debug/cs334-game.exe
```

C++23, OpenGL 3.3 Core. Default build type is Debug; pass `-DCMAKE_BUILD_TYPE=Release` to override. There are no tests beyond the empty CTest suite.

## Architecture

The project is a terrain renderer with a first-person camera. All game logic lives in [src/main.cpp](src/main.cpp) (~460 lines). Supporting utilities are header-only:

- [src/Camera.hpp](src/Camera.hpp) — first-person camera with Euler angles, mouse look, and per-frame movement deltas
- [src/Shader.hpp](src/Shader.hpp) — compiles GLSL sources, provides typed `setUniform` helpers
- [src/Constants.hpp](src/Constants.hpp) — `TERRAIN_SCALE` and related terrain-sizing constants

### Rendering Pipeline (two-pass deferred)

1. **Terrain pass** — heightmap mesh rendered to an FBO; height drives color via `terrain_fragment.glsl`
2. **Occlusion pass** — same mesh rendered to a second FBO using `occlusion_fragment.glsl` to build a sun-occlusion mask
3. **God-ray composite** — full-screen quad reads both FBOs and applies the radial blur in `godray_fragment.glsl`

Shaders are loaded at runtime from [resources/](resources/); the working directory must be the repo root.

### Terrain Mesh

Built from [resources/nashville.png](resources/nashville.png) (heightmap). `main.cpp` generates vertex/index arrays manually (C-style arrays) and uploads them once at startup. Normals are computed per-vertex from finite differences.

### Collision & Physics

Height-based only (no broad-phase). Each frame the camera Y is clamped to `terrain_height + eye_height`; gravity accumulates downward velocity when airborne. See lines ~346–371 in `main.cpp`.

## Dependencies

| Library | Location | Purpose |
|---|---|---|
| GLFW | `externals/glfw` (submodule) | Window & input |
| GLAD | `externals/glad` | OpenGL loader |
| GLM | FetchContent (v1.0.3) | Math |
| stb_image | `src/stb_image.h` | PNG loading |

After cloning, initialise submodules:
```bash
git submodule update --init --recursive
```
