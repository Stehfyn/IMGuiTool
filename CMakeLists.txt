cmake_minimum_required(VERSION 3.21)

project(IMGuiTool
  VERSION     0.1.0
  DESCRIPTION "Cross-platform Dear ImGui app (GLFW + OpenGL3)"
  LANGUAGES   C CXX
)

# --------------------------------------------------------------------------
# Project-level guards & conveniences (only when built standalone, so the
# project stays embeddable via add_subdirectory / FetchContent).
# --------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use a separate build dir, e.g. `cmake -B build`.")
  endif()

  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # tooling (clangd, etc.)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Default to Release for single-config generators (Ninja, Makefiles).
  get_property(_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(NOT _multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
  endif()
endif()

# Verify submodules are present before referencing them.
foreach(_sm IN ITEMS external/glfw/CMakeLists.txt external/imgui/imgui.cpp)
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_sm}")
    message(FATAL_ERROR
      "Missing dependency '${_sm}'. Initialize submodules:\n"
      "  git submodule update --init --recursive")
  endif()
endforeach()

# --------------------------------------------------------------------------
# Static runtime — toolchain-agnostic equivalent of MSVC /MT,/MTd.
#   MSVC / clang-cl   : static CRT via CMAKE_MSVC_RUNTIME_LIBRARY (CMP0091).
#   GCC / Clang (GNU) : static libstdc++/libgcc on the executable (see below).
# --------------------------------------------------------------------------
option(IMGUITOOL_STATIC_RUNTIME "Link the C/C++ runtime statically" ON)

if(IMGUITOOL_STATIC_RUNTIME AND MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --------------------------------------------------------------------------
# GLFW (submodule). Build only the library; it honours
# CMAKE_MSVC_RUNTIME_LIBRARY, so the static CRT propagates automatically.
# --------------------------------------------------------------------------
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)

# --------------------------------------------------------------------------
# OpenGL (prefer the modern GLVND libraries on Linux).
# --------------------------------------------------------------------------
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# --------------------------------------------------------------------------
# Dear ImGui (submodule, no upstream CMake -> we own this target).
# --------------------------------------------------------------------------
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui SYSTEM PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_compile_features(imgui PUBLIC cxx_std_11)   # ImGui's real floor

if(WIN32)
  target_sources(imgui PRIVATE ${IMGUI_DIR}/backends/imgui_impl_win32.cpp)
  # CharacterSet=Unicode equivalent: makes TCHAR/_tWinMain resolve wide.
  # PUBLIC so the executable's _tWinMain sees it too.
  target_compile_definitions(imgui PUBLIC UNICODE _UNICODE)
  # imgui_impl_win32 uses DWM/IME/GDI. MSVC pulls these via
  # #pragma comment(lib,...); GCC/Clang-GNU ignore that, so link explicitly.
  target_link_libraries(imgui PUBLIC gdi32 user32 shell32 imm32 dwmapi)
endif()

set_target_properties(imgui glfw PROPERTIES FOLDER external)

# --------------------------------------------------------------------------
# Application.
# --------------------------------------------------------------------------
add_executable(IMGuiTool WIN32 IMGuiTool/main.cpp)   # WIN32 -> GUI subsystem / WinMain (ignored off-Windows)
target_link_libraries(IMGuiTool PRIVATE imgui::imgui)
target_compile_features(IMGuiTool PRIVATE cxx_std_20)

# GNU-style frontend? (MinGW GCC, Clang GNU driver, Linux GCC/Clang) — excludes MSVC and clang-cl.
set(_gnu_frontend FALSE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
   OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
       AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
  set(_gnu_frontend TRUE)
endif()

# MinGW/Clang-GNU on Windows: UNICODE is defined, so the entry point is
# wWinMain. -municode selects the matching wide CRT startup; without it the
# MinGW runtime looks for WinMain and the link fails. (MSVC does this auto.)
if(_gnu_frontend AND WIN32)
  target_compile_options(IMGuiTool PRIVATE -municode)
  target_link_options(IMGuiTool PRIVATE -municode)
endif()

# Static libstdc++/libgcc for GNU-style frontends.
# (Skipped on Apple: its toolchain uses libc++ with no static-link story, and
#  -static-libstdc++ is rejected — the system C++ runtime is always dynamic.)
if(IMGUITOOL_STATIC_RUNTIME AND _gnu_frontend AND NOT APPLE)
  if(WIN32)
    # MinGW: link the whole runtime (libstdc++, libgcc, winpthread) statically.
    target_link_options(IMGuiTool PRIVATE -static -static-libgcc -static-libstdc++)
  else()
    # Linux/Unix: static C++ runtime only; a full -static breaks against
    # X11/Wayland/glibc, so the system C library stays dynamic.
    target_link_options(IMGuiTool PRIVATE -static-libgcc -static-libstdc++)
  endif()
endif()

# --------------------------------------------------------------------------
# Install (standalone builds only).
# --------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
  include(GNUInstallDirs)
  install(TARGETS IMGuiTool RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
