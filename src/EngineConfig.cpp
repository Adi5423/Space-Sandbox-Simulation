#include "EngineConfig.h"
#include <fstream>
#include <sstream>

EngineConfig& EngineConfig::getInstance() {
    static EngineConfig instance;
    return instance;
}

EngineConfig::EngineConfig() {
    loadConfig();
}

EngineConfig::~EngineConfig() {}

bool EngineConfig::loadConfig(const std::string& path) {
    std::lock_guard<std::mutex> lock(configMutex);
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open config file: " + path);
            return false;
        }
        file >> config;
        configPath = path;
        
        if (!validateConfig()) {
            LOG_ERROR("Invalid configuration file");
            return false;
        }
        
        LOG_INFO("Configuration loaded successfully from: " + path);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading config: " + std::string(e.what()));
        return false;
    }
}

bool EngineConfig::saveConfig(const std::string& path) {
    std::lock_guard<std::mutex> lock(configMutex);
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open config file for writing: " + path);
            return false;
        }
        file << std::setw(4) << config << std::endl;
        configPath = path;
        LOG_INFO("Configuration saved successfully to: " + path);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error saving config: " + std::string(e.what()));
        return false;
    }
}

template<typename T>
T EngineConfig::getValue(const std::string& path, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(configMutex);
    try {
        return config.value(path, defaultValue);
    } catch (const std::exception& e) {
        LOG_WARNING("Error reading config value at " + path + ": " + std::string(e.what()));
        return defaultValue;
    }
}

bool EngineConfig::validateConfig() const {
    // Check required sections
    const std::vector<std::string> requiredSections = {
        "engine", "physics", "rendering", "simulation",
        "resource_management", "debug", "input", "optimization"
    };

    for (const auto& section : requiredSections) {
        if (!config.contains(section)) {
            LOG_ERROR("Missing required section in config: " + section);
            return false;
        }
    }

    return true;
}

// Physics settings
bool EngineConfig::isGravityEnabled() const {
    return getValue("physics.gravity.enabled", true);
}

double EngineConfig::getGravityConstant() const {
    return getValue("physics.gravity.constant", 6.67430e-11);
}

double EngineConfig::getMaxGravityDistance() const {
    return getValue("physics.gravity.max_distance", 1e12);
}

bool EngineConfig::isCollisionEnabled() const {
    return getValue("physics.collision.enabled", true);
}

int EngineConfig::getCollisionIterations() const {
    return getValue("physics.collision.resolution_iterations", 4);
}

double EngineConfig::getFixedTimestep() const {
    return getValue("physics.time.fixed_timestep", 0.016666);
}

double EngineConfig::getMaxTimestep() const {
    return getValue("physics.time.max_timestep", 0.1);
}

double EngineConfig::getTimeScale() const {
    return getValue("physics.time.time_scale", 1.0);
}

// Rendering settings
int EngineConfig::getDefaultWindowWidth() const {
    return getValue("rendering.window.default_width", 1920);
}

int EngineConfig::getDefaultWindowHeight() const {
    return getValue("rendering.window.default_height", 1080);
}

bool EngineConfig::isVSyncEnabled() const {
    return getValue("rendering.window.vsync", true);
}

bool EngineConfig::isFullscreen() const {
    return getValue("rendering.window.fullscreen", true);
}

bool EngineConfig::isResizable() const {
    return getValue("rendering.window.resizable", true);
}

float EngineConfig::getFOV() const {
    return getValue("rendering.camera.fov", 45.0f);
}

float EngineConfig::getNearPlane() const {
    return getValue("rendering.camera.near_plane", 0.1f);
}

float EngineConfig::getFarPlane() const {
    return getValue("rendering.camera.far_plane", 1000.0f);
}

float EngineConfig::getCameraSpeed() const {
    return getValue("rendering.camera.movement_speed", 2.5f);
}

float EngineConfig::getSprintMultiplier() const {
    return getValue("rendering.camera.sprint_multiplier", 5.0f);
}

float EngineConfig::getCameraSensitivity() const {
    return getValue("rendering.camera.sensitivity", 0.1f);
}

// Simulation settings
int EngineConfig::getMaxObjects() const {
    return getValue("simulation.max_objects", 1000);
}

bool EngineConfig::isSpatialPartitioningEnabled() const {
    return getValue("simulation.spatial_partitioning.enabled", true);
}

double EngineConfig::getGridSize() const {
    return getValue("simulation.spatial_partitioning.grid_size", 100.0);
}

int EngineConfig::getMaxObjectsPerCell() const {
    return getValue("simulation.spatial_partitioning.max_objects_per_cell", 50);
}

int EngineConfig::getTrajectoryPredictionSteps() const {
    return getValue("simulation.trajectory.prediction_steps", 100);
}

double EngineConfig::getTrajectoryStepSize() const {
    return getValue("simulation.trajectory.step_size", 1.0);
}

double EngineConfig::getMaxPredictionTime() const {
    return getValue("simulation.trajectory.max_prediction_time", 1000.0);
}

// Resource management
size_t EngineConfig::getCacheMaxSize() const {
    return getValue("resource_management.cache.max_size", size_t(1024));
}

std::string EngineConfig::getCachePolicy() const {
    return getValue("resource_management.cache.policy", std::string("LRU"));
}

bool EngineConfig::isPreloadAssetsEnabled() const {
    return getValue("resource_management.cache.preload_assets", true);
}

size_t EngineConfig::getMaxTextureMemory() const {
    return getValue("resource_management.memory.max_texture_memory", size_t(1024));
}

size_t EngineConfig::getMaxMeshMemory() const {
    return getValue("resource_management.memory.max_mesh_memory", size_t(512));
}

size_t EngineConfig::getMaxShaderMemory() const {
    return getValue("resource_management.memory.max_shader_memory", size_t(64));
}

// Debug settings
std::string EngineConfig::getLogLevel() const {
    return getValue("debug.logging.level", std::string("INFO"));
}

std::string EngineConfig::getLogFile() const {
    return getValue("debug.logging.file", std::string("engine.log"));
}

size_t EngineConfig::getMaxLogFileSize() const {
    return getValue("debug.logging.max_file_size", size_t(10));
}

int EngineConfig::getMaxLogFiles() const {
    return getValue("debug.logging.max_files", 5);
}

bool EngineConfig::isProfilingEnabled() const {
    return getValue("debug.profiling.enabled", true);
}

double EngineConfig::getProfilingInterval() const {
    return getValue("debug.profiling.sample_interval", 0.1);
}

bool EngineConfig::isGridVisible() const {
    return getValue("debug.visualization.show_grid", true);
}

bool EngineConfig::isTrajectoryVisible() const {
    return getValue("debug.visualization.show_trajectories", true);
}

bool EngineConfig::isColliderVisible() const {
    return getValue("debug.visualization.show_colliders", false);
}

bool EngineConfig::isFPSVisible() const {
    return getValue("debug.visualization.show_fps", true);
}

// Input settings
std::string EngineConfig::getKeyBinding(const std::string& action) const {
    std::string path = "input.keyboard.movement." + action;
    std::string value = getValue(path, std::string());
    if (value.empty()) {
        path = "input.keyboard.camera." + action;
        value = getValue(path, std::string());
    }
    return value;
}

float EngineConfig::getMouseSensitivity() const {
    return getValue("input.mouse.sensitivity", 0.1f);
}

bool EngineConfig::isMouseYInverted() const {
    return getValue("input.mouse.invert_y", false);
}

// Optimization settings
int EngineConfig::getPhysicsThreads() const {
    return getValue("optimization.threading.physics_threads", 4);
}

int EngineConfig::getRenderThreads() const {
    return getValue("optimization.threading.render_threads", 1);
}

int EngineConfig::getIOThreads() const {
    return getValue("optimization.threading.io_threads", 2);
}

bool EngineConfig::isBatchingEnabled() const {
    return getValue("optimization.batching.enabled", true);
}

int EngineConfig::getMaxBatchSize() const {
    return getValue("optimization.batching.max_batch_size", 1000);
}

bool EngineConfig::isCullingEnabled() const {
    return getValue("optimization.culling.enabled", true);
}

bool EngineConfig::isFrustumCullingEnabled() const {
    return getValue("optimization.culling.frustum_culling", true);
}

bool EngineConfig::isOcclusionCullingEnabled() const {
    return getValue("optimization.culling.occlusion_culling", true);
} 