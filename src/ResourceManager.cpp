#include "ResourceManager.h"
#include <filesystem>

std::filesystem::path ResourceManager::executablePath;

void ResourceManager::initialize(const char* argv0) {
    // Get the executable's directory
    executablePath = std::filesystem::path(argv0).parent_path();
    
    // If the executable is in the build directory, go up one level to find resources
    if (executablePath.filename() == "build") {
        executablePath = executablePath.parent_path();
    }
}

std::string ResourceManager::getResourcePath(const std::string& relativePath) {
    return (executablePath / relativePath).string();
} 