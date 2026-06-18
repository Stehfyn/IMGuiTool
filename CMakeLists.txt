cmake_minimum_required(VERSION 3.21)

project(IMGuiTool LANGUAGES C CXX)

# --------------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release for single-config generators (Ninja, Makefiles).
get_property(_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT _multi_config AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Match the old MSBuild project (CharacterSet=Unicode): makes _tWinMain
# resolve to wWinMain and TCHAR map to wide on all Windows toolchains.
if(WIN32)
  add_compile_definitions(UNICODE _UNICODE)
endif()

# --------------------------------------------------------------------------
# Static runtime  (toolchain-agnostic equivalent of /MT,/MTd)
#   MSVC / clang-cl  -> static CRT via CMAKE_MSVC_RUNTIME_LIBRARY (CMP0091)
#   GCC / Clang(GNU) -> -static-libgcc/-static-libstdc++ (+winpthread) on app
# --------------------------------------------------------------------------
option(IMGUITOOL_STATIC_RUNTIME "Link the C/C++ runtime statically" ON)

if(IMGUITOOL_STATIC_RUNTIME AND MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --------------------------------------------------------------------------
# GLFW  (submodule, built from source). Its own CMake honours
# CMAKE_MSVC_RUNTIME_LIBRARY, so the static CRT propagates automatically.
# --------------------------------------------------------------------------
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# --------------------------------------------------------------------------
# OpenGL
# --------------------------------------------------------------------------
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# --------------------------------------------------------------------------
# Dear ImGui  (submodule, no upstream CMake -> we own the target)
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

target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_compile_features(imgui PUBLIC cxx_std_20)

if(WIN32)
  target_sources(imgui PRIVATE ${IMGUI_DIR}/backends/imgui_impl_win32.cpp)
  # imgui_impl_win32 pulls in DWM/IME/GDI. MSVC links these via
  # #pragma comment(lib,...); GCC/Clang-GNU ignore that pragma, so link
  # them explicitly to keep all three toolchains equivalent.
  target_link_libraries(imgui PUBLIC gdi32 user32 shell32 imm32 dwmapi)
endif()

# --------------------------------------------------------------------------
# Application
# --------------------------------------------------------------------------
add_executable(IMGuiTool WIN32 IMGuiTool/main.cpp)   # WIN32 -> GUI subsystem / WinMain
target_link_libraries(IMGuiTool PRIVATE imgui)

# Static runtime for GNU-style frontends (MinGW GCC, Clang GNU driver).
if(IMGUITOOL_STATIC_RUNTIME
   AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
            AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")))
  target_link_options(IMGuiTool PRIVATE -static -static-libgcc -static-libstdc++)
endif()
