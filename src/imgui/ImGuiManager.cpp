#include "ImGuiManager.h"
#include "World.h"
#include <GLFW/glfw3.h>

ImGuiManager::ImGuiManager(GLFWwindow* window) : m_window(window) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup style and fonts
    setupStyle();
    setupFonts();
}

ImGuiManager::~ImGuiManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(15, 15);
    style.WindowRounding = 5.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 4.0f;
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;

    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGuiManager::setupFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
}

void ImGuiManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::showMainWindow(World& world, const glm::vec3& cameraPos, const glm::vec3& cameraFront) {
    showMainMenuBar();

    // Show windows
    if (m_state.showSettings) showSettingsWindow();
    if (m_state.showDebug) showDebugWindow();
    showSimulationControls(world);
    showCameraControls(cameraPos, cameraFront);
    showBodyControls(world);
    showViewportControls();
    showTrajectorySettings();
}

void ImGuiManager::showMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Simulation")) {}
            if (ImGui::MenuItem("Load Simulation")) {}
            if (ImGui::MenuItem("Save Simulation")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(m_window, true); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Settings", nullptr, &m_state.showSettings);
            ImGui::MenuItem("Debug", nullptr, &m_state.showDebug);
            ImGui::MenuItem("Simulation Controls", nullptr, &m_state.showSimulationControls);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ImGuiManager::showSimulationControls(World& /*world*/) {
    if (!m_state.showSimulationControls) return;

    ImGui::Begin("Simulation Controls", &m_state.showSimulationControls);
    
    ImGui::Text("Simulation Settings");
    ImGui::Separator();
    
    ImGui::Checkbox("Pause Simulation", &m_state.pauseSimulation);
    ImGui::SliderFloat("Simulation Speed", &m_state.simulationSpeed, 0.1f, 10.0f, "%.1fx");
    ImGui::SliderFloat("Time Scale", &m_state.timeScale, 0.1f, 100.0f, "%.1fx");
    
    ImGui::Separator();
    ImGui::Text("Visualization");
    ImGui::Checkbox("Show Grid", &m_state.showGrid);
    ImGui::Checkbox("Show Trajectories", &m_state.showTrajectories);
    
    ImGui::End();
}

void ImGuiManager::showCameraControls(const glm::vec3& cameraPos, const glm::vec3& cameraFront) {
    ImGui::Begin("Camera Controls");
    
    ImGui::Text("Camera Position");
    float pos[3] = { cameraPos.x, cameraPos.y, cameraPos.z };
    if (ImGui::DragFloat3("Position", pos, 0.1f)) {
        // Position is read-only in this version
    }
    
    ImGui::Text("Camera Direction");
    float dir[3] = { cameraFront.x, cameraFront.y, cameraFront.z };
    if (ImGui::DragFloat3("Direction", dir, 0.1f)) {
        // Direction is read-only in this version
    }
    
    ImGui::End();
}

void ImGuiManager::showBodyControls(World& world) {
    ImGui::Begin("Celestial Bodies");
    
    for (size_t i = 0; i < world.getBodyCount(); ++i) {
        Body& body = world.getBody(i);
        if (ImGui::CollapsingHeader(("Body " + std::to_string(i)).c_str())) {
            float pos[3] = { static_cast<float>(body.position.x), 
                           static_cast<float>(body.position.y), 
                           static_cast<float>(body.position.z) };
            if (ImGui::DragFloat3("Position", pos, 0.1f)) {
                body.position = Vector(pos[0], pos[1], pos[2]);
            }
            
            float vel[3] = { static_cast<float>(body.velocity.x), 
                           static_cast<float>(body.velocity.y), 
                           static_cast<float>(body.velocity.z) };
            if (ImGui::DragFloat3("Velocity", vel, 0.1f)) {
                body.velocity = Vector(vel[0], vel[1], vel[2]);
            }
            
            float mass = static_cast<float>(body.mass);
            if (ImGui::DragFloat("Mass", &mass, 0.1f)) {
                body.mass = mass;
            }
        }
    }
    
    ImGui::End();
}

void ImGuiManager::showDebugWindow() {
    if (!m_state.showDebug) return;

    ImGui::Begin("Debug Window", &m_state.showDebug);
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, 
                ImGui::GetIO().Framerate);
    
    ImGui::Separator();
    ImGui::Text("Memory Usage");
    // Add memory usage information here
    
    ImGui::End();
}

void ImGuiManager::showSettingsWindow() {
    if (!m_state.showSettings) return;

    ImGui::Begin("Settings", &m_state.showSettings);
    
    if (ImGui::CollapsingHeader("Graphics")) {
        // Add graphics settings here
    }
    
    if (ImGui::CollapsingHeader("Physics")) {
        // Add physics settings here
    }
    
    if (ImGui::CollapsingHeader("Input")) {
        // Add input settings here
    }
    
    ImGui::End();
}

void ImGuiManager::showViewportControls() {
    ImGui::Begin("Viewport");
    
    // Add viewport controls here
    
    ImGui::End();
}

void ImGuiManager::showTrajectorySettings() {
    if (!m_state.showTrajectories) return;

    ImGui::Begin("Trajectory Settings", &m_state.showTrajectories);
    
    // Add trajectory settings here
    
    ImGui::End();
}

void ImGuiManager::showHelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ImGuiManager::showTooltip(const char* desc) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
} 