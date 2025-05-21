#pragma once
#include <string>
#include <filesystem>

class ResourceManager {
public:
    static void initialize(const char* argv0);
    static std::string getResourcePath(const std::string& relativePath);

private:
    static std::filesystem::path executablePath;
}; 