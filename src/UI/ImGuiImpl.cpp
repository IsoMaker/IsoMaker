/**
 * @file ImGuiImpl.cpp
 * @brief Implementation file for Dear ImGui
 * @details This file contains the implementation for the Dear ImGui integration with raylib.
 */

#define IMGUI_ENABLE_DOCKING 1
#include "raylib.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "rlgl.h"
#include "UITheme.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>
#include <stdexcept>

// Get GLFW window handle from raylib window
GLFWwindow* GetGLFWWindowHandle() {
    // Try to get the window handle using glfwGetCurrentContext
    // This should work since raylib uses GLFW internally
    return glfwGetCurrentContext();
}

namespace UI {

// ImGui context
bool imguiInitialized = false;

// Initialize ImGui
void ImGuiInit() {
    if (imguiInitialized) return;
    
    // Add a small delay to ensure the window is fully initialized
    // This helps with stability
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    
    try {
        // Setup Platform/Renderer backends
        GLFWwindow* window = GetGLFWWindowHandle();
        if (!window) {
            TraceLog(LOG_ERROR, "IMGUI: Failed to get GLFW window handle");
            return;
        }
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Convert colors from UITheme
        ImVec4 primary = ImVec4(UI_PRIMARY.r/255.0f, UI_PRIMARY.g/255.0f, UI_PRIMARY.b/255.0f, UI_PRIMARY.a/255.0f);
        ImVec4 secondary = ImVec4(UI_SECONDARY.r/255.0f, UI_SECONDARY.g/255.0f, UI_SECONDARY.b/255.0f, UI_SECONDARY.a/255.0f);
        ImVec4 background = ImVec4(BACKGROUND.r/255.0f, BACKGROUND.g/255.0f, BACKGROUND.b/255.0f, BACKGROUND.a/255.0f);
        ImVec4 textPrimary = ImVec4(UI_TEXT_PRIMARY.r/255.0f, UI_TEXT_PRIMARY.g/255.0f, UI_TEXT_PRIMARY.b/255.0f, UI_TEXT_PRIMARY.a/255.0f);
        ImVec4 textSecondary = ImVec4(UI_TEXT_SECONDARY.r/255.0f, UI_TEXT_SECONDARY.g/255.0f, UI_TEXT_SECONDARY.b/255.0f, UI_TEXT_SECONDARY.a/255.0f);
        ImVec4 accentPrimary = ImVec4(ACCENT_PRIMARY.r/255.0f, ACCENT_PRIMARY.g/255.0f, ACCENT_PRIMARY.b/255.0f, ACCENT_PRIMARY.a/255.0f);
        ImVec4 accentSecondary = ImVec4(ACCENT_SECONDARY.r/255.0f, ACCENT_SECONDARY.g/255.0f, ACCENT_SECONDARY.b/255.0f, ACCENT_SECONDARY.a/255.0f);
        ImVec4 accentTertiary = ImVec4(ACCENT_TERTIARY.r/255.0f, ACCENT_TERTIARY.g/255.0f, ACCENT_TERTIARY.b/255.0f, ACCENT_TERTIARY.a/255.0f);
        
        // Window style
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(5.0f, 4.0f);
        style.CellPadding = ImVec2(4.0f, 2.0f);
        style.ItemSpacing = ImVec2(6.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
        style.IndentSpacing = 20.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.TabBorderSize = 0.0f;
        style.WindowRounding = 6.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 2.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabRounding = 4.0f;
        style.LogSliderDeadzone = 4.0f;
        style.TabRounding = 4.0f;
        
        // Colors
        style.Colors[ImGuiCol_Text] = textPrimary;
        style.Colors[ImGuiCol_TextDisabled] = textSecondary;
        style.Colors[ImGuiCol_WindowBg] = secondary;
        style.Colors[ImGuiCol_ChildBg] = primary;
        style.Colors[ImGuiCol_PopupBg] = primary;
        style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = primary;
        style.Colors[ImGuiCol_FrameBgHovered] = accentTertiary;
        style.Colors[ImGuiCol_FrameBgActive] = accentPrimary;
        style.Colors[ImGuiCol_TitleBg] = primary;
        style.Colors[ImGuiCol_TitleBgActive] = accentPrimary;
        style.Colors[ImGuiCol_TitleBgCollapsed] = primary;
        style.Colors[ImGuiCol_MenuBarBg] = primary;
        style.Colors[ImGuiCol_ScrollbarBg] = primary;
        style.Colors[ImGuiCol_ScrollbarGrab] = secondary;
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = accentTertiary;
        style.Colors[ImGuiCol_ScrollbarGrabActive] = accentPrimary;
        style.Colors[ImGuiCol_CheckMark] = accentPrimary;
        style.Colors[ImGuiCol_SliderGrab] = accentTertiary;
        style.Colors[ImGuiCol_SliderGrabActive] = accentPrimary;
        style.Colors[ImGuiCol_Button] = primary;
        style.Colors[ImGuiCol_ButtonHovered] = accentTertiary;
        style.Colors[ImGuiCol_ButtonActive] = accentPrimary;
        style.Colors[ImGuiCol_Header] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = accentTertiary;
        style.Colors[ImGuiCol_HeaderActive] = accentPrimary;
        style.Colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered] = accentTertiary;
        style.Colors[ImGuiCol_SeparatorActive] = accentPrimary;
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.33f, 0.33f, 0.33f, 0.25f);
        style.Colors[ImGuiCol_ResizeGripHovered] = accentTertiary;
        style.Colors[ImGuiCol_ResizeGripActive] = accentPrimary;
        style.Colors[ImGuiCol_Tab] = primary;
        style.Colors[ImGuiCol_TabHovered] = accentTertiary;
        style.Colors[ImGuiCol_TabActive] = accentPrimary;
        style.Colors[ImGuiCol_TabUnfocused] = primary;
        style.Colors[ImGuiCol_TabUnfocusedActive] = accentSecondary;
        style.Colors[ImGuiCol_DockingPreview] = accentTertiary;
        style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = accentPrimary;
        style.Colors[ImGuiCol_PlotLinesHovered] = accentSecondary;
        style.Colors[ImGuiCol_PlotHistogram] = accentPrimary;
        style.Colors[ImGuiCol_PlotHistogramHovered] = accentSecondary;
        style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        style.Colors[ImGuiCol_TextSelectedBg] = accentTertiary;
        style.Colors[ImGuiCol_DragDropTarget] = accentPrimary;
        style.Colors[ImGuiCol_NavHighlight] = accentPrimary;
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        
        imguiInitialized = true;
        TraceLog(LOG_INFO, "IMGUI: Initialized successfully");
    } 
    catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "IMGUI: Exception during initialization: %s", e.what());
        ImGui::DestroyContext();
        return;
    }
    catch (...) {
        TraceLog(LOG_ERROR, "IMGUI: Unknown exception during initialization");
        ImGui::DestroyContext();
        return;
    }
}

// Shutdown ImGui
void ImGuiShutdown() {
    if (!imguiInitialized) return;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    imguiInitialized = false;
}

// Start ImGui frame
void ImGuiBeginFrame() {
    // Try to initialize if not already initialized
    if (!imguiInitialized) {
        ImGuiInit();
        if (!imguiInitialized) {
            TraceLog(LOG_WARNING, "IMGUI: Skipping frame rendering due to initialization failure");
            return;
        }
    }
    
    try {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    catch (...) {
        TraceLog(LOG_ERROR, "IMGUI: Exception during frame initialization");
    }
}

// End ImGui frame and render
void ImGuiEndFrame() {
    if (!imguiInitialized) return;
    
    try {
        ImGui::Render();
        
        // Draw to main window
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    catch (...) {
        TraceLog(LOG_ERROR, "IMGUI: Exception during frame rendering");
    }
}

} // namespace UI