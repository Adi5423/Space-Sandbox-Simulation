#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <memory>
#include <functional>
#include <filesystem>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace fs = std::filesystem;

/*
 * EngineBackend.h
 * 
 * This file contains the core backend systems for the game engine, including:
 * 1. Logging System - For tracking engine events, errors, and debugging
 * 2. Cache System - For efficient resource management and memory optimization
 * 3. File System - For handling file operations and resource loading
 * 4. Thread Pool - For managing concurrent operations
 * 5. Event System - For handling engine events and callbacks
 * 6. Performance Monitoring - For tracking engine performance metrics
 * 7. Error Handling - For robust error management and recovery
 * 8. Configuration Management - For engine settings and user preferences
 */

// Log levels for the logging system
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// Cache policy for the caching system
enum class CachePolicy {
    LRU,    // Least Recently Used
    FIFO,   // First In First Out
    LFU     // Least Frequently Used
};

class Logger {
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message, const std::string& file = "", int line = 0);
    void setLogFile(const std::string& path);
    void setLogLevel(LogLevel level);
    void flush();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile;
    LogLevel currentLevel;
    mutable std::mutex logMutex;
    std::string getTimestamp();
    std::string getLevelString(LogLevel level);
};

template<typename T>
class Cache {
public:
    Cache(size_t maxSize, CachePolicy policy = CachePolicy::LRU);
    void put(const std::string& key, const T& value);
    bool get(const std::string& key, T& value);
    void remove(const std::string& key);
    void clear();
    size_t size() const;

private:
    struct CacheEntry {
        T value;
        std::chrono::system_clock::time_point lastAccess;
        size_t accessCount;
    };

    std::unordered_map<std::string, CacheEntry> cache;
    size_t maxSize;
    CachePolicy policy;
    mutable std::mutex cacheMutex;
    void evict();
};

class FileSystem {
public:
    static FileSystem& getInstance();
    bool loadFile(const std::string& path, std::vector<uint8_t>& data);
    bool saveFile(const std::string& path, const std::vector<uint8_t>& data);
    bool fileExists(const std::string& path);
    std::string getFileExtension(const std::string& path);
    std::string getFileName(const std::string& path);
    void createDirectory(const std::string& path);
    std::vector<std::string> listFiles(const std::string& path, const std::string& extension = "");

private:
    FileSystem();
    ~FileSystem();
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
};

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args);
    void waitForCompletion();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

class EventSystem {
public:
    using EventCallback = std::function<void(const void*)>;
    
    void subscribe(const std::string& eventName, EventCallback callback);
    void unsubscribe(const std::string& eventName, EventCallback callback);
    void emit(const std::string& eventName, const void* data = nullptr);

private:
    std::unordered_map<std::string, std::vector<EventCallback>> eventHandlers;
    mutable std::mutex eventMutex;
};

class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    void startFrame();
    void endFrame();
    void startOperation(const std::string& name);
    void endOperation(const std::string& name);
    double getFPS() const;
    double getOperationTime(const std::string& name) const;
    void reset();

private:
    PerformanceMonitor();
    ~PerformanceMonitor();
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;

    std::chrono::system_clock::time_point frameStart;
    std::unordered_map<std::string, double> operationTimes;
    mutable std::mutex monitorMutex;
    double fps;
    int frameCount;
    std::chrono::system_clock::time_point lastFPSUpdate;
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    void handleError(const std::string& error, bool fatal = false);
    void setErrorCallback(std::function<void(const std::string&, bool)> callback);
    bool hasError() const;
    std::string getLastError() const;
    void clearError();

private:
    ErrorHandler();
    ~ErrorHandler();
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    std::function<void(const std::string&, bool)> errorCallback;
    std::string lastError;
    bool errorOccurred;
    mutable std::mutex errorMutex;
};

class ConfigManager {
public:
    static ConfigManager& getInstance();
    bool loadConfig(const std::string& path);
    bool saveConfig(const std::string& path);
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    bool hasKey(const std::string& key) const;
    void clear();

private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    std::unordered_map<std::string, std::string> config;
    mutable std::mutex configMutex;
};

// Convenience macros for logging
#define LOG_DEBUG(msg) Logger::getInstance().log(LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::getInstance().log(LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::getInstance().log(LogLevel::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::getInstance().log(LogLevel::ERROR, msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) Logger::getInstance().log(LogLevel::FATAL, msg, __FILE__, __LINE__) 