#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#include "imgui_impl_win32.h"
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace ImGui
{
    void Init(GLFWwindow* window, float main_scale);
    void BeginFrame(void);
    void DrawFrame(void);
    void EndFrame(GLFWwindow* window);
    void Cleanup(void);
}

// Platform-agnostic application body. Entry points below dispatch into this.
static int RunApp(void)
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // required for a 3.3 core context on macOS
#endif

    // Create window with graphics context
    // Get DPI scale before creating window
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Dear ImGui + Docking", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    ImGui::Init(window, main_scale);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start ImGui frame
        ImGui::BeginFrame();

        // Draw
        ImGui::DrawFrame();

        // End ImGui frame
        ImGui::EndFrame(window);
    }

    // Cleanup
    ImGui::Cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#ifdef _WIN32
int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    // Fix DPI awareness first (Win32-only; other platforms scale via GLFW content scale)
    ImGui_ImplWin32_EnableDpiAwareness();

    return RunApp();
}
#else
int main(void)
{
    return RunApp();
}
#endif

namespace ImGui
{
    void Init(GLFWwindow* window, float main_scale)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Enable Docking (requires docking branch!)
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Optional: multi-viewport

        // Apply DPI scaling
        io.FontGlobalScale = main_scale;
        ImGui::GetStyle().ScaleAllSizes(main_scale);
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        ImGui::StyleColorsDark();

        // When viewports are enabled, tweak WindowRounding so platform
        // windows look identical to regular ones
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
          style.WindowRounding = 0.0f;
          style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        style.FontScaleDpi = main_scale;

        // Initialize backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void BeginFrame(void)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void DrawFrame(void)
    {
        // Enable full-screen dockspace (optional but common pattern)
        ImGui::DockSpaceOverViewport();

        // Your windows go here
        ImGui::ShowDemoWindow();
    }

    void EndFrame(GLFWwindow* window)
    {
        int display_w;
        int display_h;
        const ImGuiIO& io = ImGui::GetIO();

        // Render
        ImGui::Render();

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Required for multi-viewport support
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    void Cleanup(void)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
