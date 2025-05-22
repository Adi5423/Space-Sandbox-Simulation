#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "EngineBackend.h"

using json = nlohmann::json;

class EngineConfig {
public:
    static EngineConfig& getInstance();

    // Load and save configuration
    bool loadConfig(const std::string& path = "engine_config.json");
    bool saveConfig(const std::string& path = "engine_config.json");

    // Physics settings
    bool isGravityEnabled() const;
    double getGravityConstant() const;
    double getMaxGravityDistance() const;
    bool isCollisionEnabled() const;
    int getCollisionIterations() const;
    double getFixedTimestep() const;
    double getMaxTimestep() const;
    double getTimeScale() const;

    // Rendering settings
    int getDefaultWindowWidth() const;
    int getDefaultWindowHeight() const;
    bool isVSyncEnabled() const;
    bool isFullscreen() const;
    bool isResizable() const;
    float getFOV() const;
    float getNearPlane() const;
    float getFarPlane() const;
    float getCameraSpeed() const;
    float getSprintMultiplier() const;
    float getCameraSensitivity() const;

    // Simulation settings
    int getMaxObjects() const;
    bool isSpatialPartitioningEnabled() const;
    double getGridSize() const;
    int getMaxObjectsPerCell() const;
    int getTrajectoryPredictionSteps() const;
    double getTrajectoryStepSize() const;
    double getMaxPredictionTime() const;

    // Resource management
    size_t getCacheMaxSize() const;
    std::string getCachePolicy() const;
    bool isPreloadAssetsEnabled() const;
    size_t getMaxTextureMemory() const;
    size_t getMaxMeshMemory() const;
    size_t getMaxShaderMemory() const;

    // Debug settings
    std::string getLogLevel() const;
    std::string getLogFile() const;
    size_t getMaxLogFileSize() const;
    int getMaxLogFiles() const;
    bool isProfilingEnabled() const;
    double getProfilingInterval() const;
    bool isGridVisible() const;
    bool isTrajectoryVisible() const;
    bool isColliderVisible() const;
    bool isFPSVisible() const;

    // Input settings
    std::string getKeyBinding(const std::string& action) const;
    float getMouseSensitivity() const;
    bool isMouseYInverted() const;

    // Optimization settings
    int getPhysicsThreads() const;
    int getRenderThreads() const;
    int getIOThreads() const;
    bool isBatchingEnabled() const;
    int getMaxBatchSize() const;
    bool isCullingEnabled() const;
    bool isFrustumCullingEnabled() const;
    bool isOcclusionCullingEnabled() const;

private:
    EngineConfig();
    ~EngineConfig();
    EngineConfig(const EngineConfig&) = delete;
    EngineConfig& operator=(const EngineConfig&) = delete;

    json config;
    std::string configPath;
    mutable std::mutex configMutex;

    // Helper methods
    template<typename T>
    T getValue(const std::string& path, const T& defaultValue) const;
    bool validateConfig() const;
}; 