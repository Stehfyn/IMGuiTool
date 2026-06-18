# IMGuiTool

[![CI](https://github.com/Stehfyn/IMGuiTool/actions/workflows/ci.yml/badge.svg)](https://github.com/Stehfyn/IMGuiTool/actions/workflows/ci.yml)

A cross-platform [Dear ImGui](https://github.com/ocornut/imgui) app (GLFW + OpenGL3).

## Toolchains

Every row is built in CI on each push.

| Platform | Compilers                  | Build systems   | Presets                              |
| -------- | -------------------------- | --------------- | ------------------------------------ |
| Windows  | MSVC, clang-cl, MinGW-w64  | Ninja, MSBuild  | `msvc` `clang-cl` `mingw` `vs`       |
| Linux    | GCC, Clang                 | Ninja, Make     | `linux-gcc` `linux-clang`            |
| macOS    | Clang                      | Ninja, Make     | `macos-clang`                        |

---

## Build (CMake ≥ 3.21)

```bash
git clone --recursive https://github.com/Stehfyn/IMGuiTool
cmake --preset <preset>
cmake --build --preset <preset>-release
```

- **Output:** `build/<preset>/<config>/IMGuiTool` (`.exe` on Windows)
- **Static runtime** by default — opt out with `-D IMGUITOOL_STATIC_RUNTIME=OFF`

### Already cloned, or building without presets

```bash
git submodule update --init --recursive               # if you cloned non-recursively
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Release && cmake --build build
```

---

## Linux packages (Debian/Ubuntu)

```bash
scripts/install-deps.sh    # GLFW needs X11/Wayland/OpenGL dev headers
```

### No apt? (offline, locked-down, embedded)

On any Debian/Ubuntu box with apt, download the `.deb` files:

```bash
scripts/get-deps.sh        # writes ./debs
```

Copy `debs/` to the target and install with `dpkg` — no network, repo, or apt needed there:

```bash
sudo dpkg -i debs/*.deb
```

On another distro, see `scripts/packages.txt` for the package list.
