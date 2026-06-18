# IMGuiTool

Native Windows [Dear ImGui](https://github.com/ocornut/imgui) app (GLFW + OpenGL3). Dependencies are submodules, built from source as static libs.

## How to build (Visual Studio)
- `Visual Studio 2026` with **Desktop development with C++** (projects target the `v145` toolset).

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
Open `IMGuiTool.slnx` and build (`Platform`: `x64`/`x86`, `Configuration`: `Debug`/`Release`).

Or via `Developer Command Prompt for VS`:
```bash
cd IMGuiTool
msbuild /m /p:Configuration=Release /p:Platform=x64 IMGuiTool.slnx
```
