#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>

class Logger {
public:
    Logger(const std::string& filename);
    ~Logger();
    void log(const std::string& event);
    void logTrade(const std::string& tradeInfo);
private:
    std::ofstream file_;
    std::mutex mtx_;
};
