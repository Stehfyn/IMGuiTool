# IMGuiTool

Cross-platform [Dear ImGui](https://github.com/ocornut/imgui) app (GLFW + OpenGL3). Runs on **Windows** and **Linux**. Dependencies are submodules, built from source as static libs. Builds with **MSVC**, **Clang/clang-cl**, **MinGW-w64 GCC** (Windows) or **GCC/Clang** (Linux); CMake is the single source of truth.

## How to build (CMake)
Requirements:
- `CMake` 3.21+ and a generator (`Ninja` recommended; the VS generator also works).
- `Git` (dependencies are submodules).
- A compiler: MSVC (`Visual Studio 2026`, Desktop C++), `clang-cl`/`clang`, MinGW-w64 `gcc` (Windows), or `gcc`/`clang` (Linux).
- **Linux only** — GLFW's build deps:
  ```bash
  sudo apt-get update && sudo apt-get install -y \
    build-essential ninja-build pkg-config \
    libgl1-mesa-dev xorg-dev libxkbcommon-dev \
    libwayland-dev wayland-protocols
  ```

The runtime is linked **statically** by default (`/MT` on MSVC/clang-cl, `-static-lib*` on GNU; on Linux the system C library stays dynamic). Disable with `-D IMGUITOOL_STATIC_RUNTIME=OFF`.

### Build Steps
Clone with submodules:
```bash
git clone --recursive https://github.com/Stehfyn/IMGuiTool
```
Or, if already cloned:
```bash
cd IMGuiTool
git submodule update --init --recursive
```
Configure and build with a preset. Windows: `msvc`, `clang-cl`, `mingw`, `vs`. Linux: `linux-gcc`, `linux-clang`.
```bash
# Windows
cmake --preset msvc
cmake --build --preset msvc-release

# Linux
cmake --preset linux-gcc
cmake --build --preset linux-gcc-release
```
The binary lands in `build/<preset>/<Config>/` (`IMGuiTool.exe` on Windows, `IMGuiTool` on Linux).

Without presets, any generator works the same way:
```bash
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Release
cmake --build build
```
