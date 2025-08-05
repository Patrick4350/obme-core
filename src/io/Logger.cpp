#include "Logger.h"
#include <iomanip>
#include <sstream>

Logger::Logger(const std::string& filename) {
    file_.open(filename, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
    
    // Log initialization
    log("Logger initialized - " + filename);
}

Logger::~Logger() {
    if (file_.is_open()) {
        log("Logger shutting down");
        file_.close();
    }
}

void Logger::log(const std::string& event) {
    std::lock_guard<std::mutex> lock(mtx_);
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
    oss << event << std::endl;
    
    file_ << oss.str();
    file_.flush();
}

void Logger::logTrade(const std::string& tradeInfo) {
    log("TRADE: " + tradeInfo);
}
