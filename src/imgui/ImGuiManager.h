#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

class World;

class ImGuiManager {
public:
    ImGuiManager(GLFWwindow* window);
    ~ImGuiManager();

    void beginFrame();
    void endFrame();
    void showMainWindow(World& world, const glm::vec3& cameraPos, const glm::vec3& cameraFront);

    // GUI State
    struct GUIState {
        bool showSettings = true;
        bool showDebug = false;
        bool showSimulationControls = true;
        float simulationSpeed = 1.0f;
        bool pauseSimulation = false;
        bool showGrid = true;
        bool showTrajectories = false;
        float timeScale = 1.0f;
    };

    GUIState& getState() { return m_state; }

private:
    void showMainMenuBar();
    void showSimulationControls(World& world);
    void showCameraControls(const glm::vec3& cameraPos, const glm::vec3& cameraFront);
    void showBodyControls(World& world);
    void showDebugWindow();
    void showSettingsWindow();
    void showViewportControls();
    void showTrajectorySettings();

    GLFWwindow* m_window;
    GUIState m_state;
    
    // Style settings
    void setupStyle();
    void setupFonts();
    
    // Utility functions
    void showHelpMarker(const char* desc);
    void showTooltip(const char* desc);
}; 