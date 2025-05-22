#include "EngineBackend.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <future>

// Logger Implementation
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : currentLevel(LogLevel::INFO) {
    setLogFile("engine.log");
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLogFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(path, std::ios::app);
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    if (level < currentLevel) return;

    std::lock_guard<std::mutex> lock(logMutex);
    std::stringstream ss;
    ss << getTimestamp() << " [" << getLevelString(level) << "] ";
    if (!file.empty()) {
        ss << file << ":" << line << " - ";
    }
    ss << message << std::endl;

    if (logFile.is_open()) {
        logFile << ss.str();
        logFile.flush();
    }
    std::cout << ss.str();
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.flush();
    }
}

// Cache Implementation
template<typename T>
Cache<T>::Cache(size_t maxSize, CachePolicy policy) 
    : maxSize(maxSize), policy(policy) {}

template<typename T>
void Cache<T>::put(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (cache.size() >= maxSize) {
        evict();
    }
    CacheEntry entry{value, std::chrono::system_clock::now(), 0};
    cache[key] = entry;
}

template<typename T>
bool Cache<T>::get(const std::string& key, T& value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto it = cache.find(key);
    if (it != cache.end()) {
        it->second.lastAccess = std::chrono::system_clock::now();
        it->second.accessCount++;
        value = it->second.value;
        return true;
    }
    return false;
}

template<typename T>
void Cache<T>::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    cache.erase(key);
}

template<typename T>
void Cache<T>::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    cache.clear();
}

template<typename T>
size_t Cache<T>::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex);
    return cache.size();
}

template<typename T>
void Cache<T>::evict() {
    switch (policy) {
        case CachePolicy::LRU: {
            auto oldest = std::min_element(cache.begin(), cache.end(),
                [](const auto& a, const auto& b) {
                    return a.second.lastAccess < b.second.lastAccess;
                });
            if (oldest != cache.end()) {
                cache.erase(oldest);
            }
            break;
        }
        case CachePolicy::LFU: {
            auto leastFrequent = std::min_element(cache.begin(), cache.end(),
                [](const auto& a, const auto& b) {
                    return a.second.accessCount < b.second.accessCount;
                });
            if (leastFrequent != cache.end()) {
                cache.erase(leastFrequent);
            }
            break;
        }
        case CachePolicy::FIFO: {
            cache.erase(cache.begin());
            break;
        }
    }
}

// FileSystem Implementation
FileSystem& FileSystem::getInstance() {
    static FileSystem instance;
    return instance;
}

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

bool FileSystem::loadFile(const std::string& path, std::vector<uint8_t>& data) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            LOG_ERROR("Failed to open file: " + path);
            return false;
        }
        data = std::vector<uint8_t>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading file " + path + ": " + e.what());
        return false;
    }
}

bool FileSystem::saveFile(const std::string& path, const std::vector<uint8_t>& data) {
    try {
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            LOG_ERROR("Failed to create file: " + path);
            return false;
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error saving file " + path + ": " + e.what());
        return false;
    }
}

bool FileSystem::fileExists(const std::string& path) {
    return fs::exists(path);
}

std::string FileSystem::getFileExtension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string FileSystem::getFileName(const std::string& path) {
    return fs::path(path).filename().string();
}

void FileSystem::createDirectory(const std::string& path) {
    fs::create_directories(path);
}

std::vector<std::string> FileSystem::listFiles(const std::string& path, const std::string& extension) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (extension.empty() || entry.path().extension() == extension) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error listing files in " + path + ": " + e.what());
    }
    return files;
}

// ThreadPool Implementation
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });
                    if (stop && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

template<class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args) {
    auto task = std::make_shared<std::packaged_task<void()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
}

void ThreadPool::waitForCompletion() {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this] { return tasks.empty(); });
}

// EventSystem Implementation
void EventSystem::subscribe(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(eventMutex);
    eventHandlers[eventName].push_back(callback);
}

void EventSystem::unsubscribe(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(eventMutex);
    auto& handlers = eventHandlers[eventName];
    handlers.erase(
        std::remove_if(handlers.begin(), handlers.end(),
            [&callback](const EventCallback& h) {
                return h.target_type() == callback.target_type();
            }),
        handlers.end()
    );
}

void EventSystem::emit(const std::string& eventName, const void* data) {
    std::lock_guard<std::mutex> lock(eventMutex);
    auto it = eventHandlers.find(eventName);
    if (it != eventHandlers.end()) {
        for (const auto& handler : it->second) {
            handler(data);
        }
    }
}

// PerformanceMonitor Implementation
PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

PerformanceMonitor::PerformanceMonitor() 
    : fps(0.0), frameCount(0), lastFPSUpdate(std::chrono::system_clock::now()) {}

PerformanceMonitor::~PerformanceMonitor() {}

void PerformanceMonitor::startFrame() {
    frameStart = std::chrono::system_clock::now();
}

void PerformanceMonitor::endFrame() {
    frameCount++;
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFPSUpdate).count();
    
    if (elapsed >= 1000) {
        fps = frameCount * 1000.0 / elapsed;
        frameCount = 0;
        lastFPSUpdate = now;
    }
}

void PerformanceMonitor::startOperation(const std::string& name) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    operationTimes[name] = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void PerformanceMonitor::endOperation(const std::string& name) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    auto end = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    auto it = operationTimes.find(name);
    if (it != operationTimes.end()) {
        it->second = end - it->second;
    }
}

double PerformanceMonitor::getFPS() const {
    return fps;
}

double PerformanceMonitor::getOperationTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    auto it = operationTimes.find(name);
    return it != operationTimes.end() ? it->second / 1000.0 : 0.0;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(monitorMutex);
    operationTimes.clear();
    frameCount = 0;
    fps = 0.0;
}

// ErrorHandler Implementation
ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() : errorOccurred(false) {}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::handleError(const std::string& error, bool fatal) {
    std::lock_guard<std::mutex> lock(errorMutex);
    lastError = error;
    errorOccurred = true;
    if (errorCallback) {
        errorCallback(error, fatal);
    }
    if (fatal) {
        LOG_FATAL(error);
        std::terminate();
    } else {
        LOG_ERROR(error);
    }
}

void ErrorHandler::setErrorCallback(std::function<void(const std::string&, bool)> callback) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorCallback = callback;
}

bool ErrorHandler::hasError() const {
    std::lock_guard<std::mutex> lock(errorMutex);
    return errorOccurred;
}

std::string ErrorHandler::getLastError() const {
    std::lock_guard<std::mutex> lock(errorMutex);
    return lastError;
}

void ErrorHandler::clearError() {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorOccurred = false;
    lastError.clear();
}

// ConfigManager Implementation
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {}
ConfigManager::~ConfigManager() {}

bool ConfigManager::loadConfig(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file) {
            LOG_ERROR("Failed to open config file: " + path);
            return false;
        }
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                config[key] = value;
            }
        }
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading config file " + path + ": " + e.what());
        return false;
    }
}

bool ConfigManager::saveConfig(const std::string& path) {
    try {
        std::ofstream file(path);
        if (!file) {
            LOG_ERROR("Failed to create config file: " + path);
            return false;
        }
        for (const auto& [key, value] : config) {
            file << key << "=" << value << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error saving config file " + path + ": " + e.what());
        return false;
    }
}

void ConfigManager::setValue(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    config[key] = value;
}

std::string ConfigManager::getValue(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(configMutex);
    auto it = config.find(key);
    return it != config.end() ? it->second : defaultValue;
}

bool ConfigManager::hasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    return config.find(key) != config.end();
}

void ConfigManager::clear() {
    std::lock_guard<std::mutex> lock(configMutex);
    config.clear();
} 