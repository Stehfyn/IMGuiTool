# IMGuiTool

Native Windows [Dear ImGui](https://github.com/ocornut/imgui) app (GLFW + OpenGL3). Dependencies are submodules, built from source as static libs. Builds with **MSVC**, **Clang**, or **MinGW-w64 GCC**; CMake is the single source of truth.

## How to build (CMake)
Requirements:
- `CMake` 3.21+ and a generator (`Ninja` recommended; the VS generator also works).
- One of: MSVC (`Visual Studio 2026`, Desktop C++), `clang-cl`/`clang`, or MinGW-w64 `gcc`.
- `Git` (dependencies are submodules).

The runtime is linked **statically** by default (`/MT` on MSVC/clang-cl, `-static-lib*` on GNU). Disable with `-D IMGUITOOL_STATIC_RUNTIME=OFF`.

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
Configure and build with a preset (`msvc`, `clang-cl`, `mingw`, or `vs`):
```bash
cmake --preset msvc
cmake --build --preset msvc-release
```
The executable lands in `build/<preset>/<Config>/IMGuiTool.exe`.

Without presets, any generator works the same way:
```bash
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Release
cmake --build build
```
