#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <cmath>

namespace Utils {
    // Timestamp utilities
    inline long getTimestamp() {
        return std::chrono::system_clock::now().time_since_epoch().count();
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint);
    uint64_t getMicrosecondsSinceEpoch();
    uint64_t getNanosecondsSinceEpoch();
    
    // Formatting utilities
    std::string formatPrice(double price);
    std::string formatQuantity(uint32_t quantity);
    std::string formatBytes(uint64_t bytes);
    std::string formatDuration(std::chrono::milliseconds duration);
    
    // Financial utilities
    double roundToTickSize(double price, double tickSize);
    bool isValidPrice(double price);
    bool isValidQuantity(uint32_t quantity);
    double calculateNotionalValue(double price, uint32_t quantity);
    double calculateWeightedAveragePrice(const std::vector<std::pair<double, uint32_t>>& fills);
    bool isWithinPercentage(double value1, double value2, double percentage);
    
    // String utilities
    std::string toUpperCase(const std::string& str);
    std::string toLowerCase(const std::string& str);
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Random data generation
    std::string generateOrderId();
    std::string getRandomSymbol();
    double getRandomPrice(double min = 90.0, double max = 110.0);
    uint32_t getRandomQuantity(uint32_t min = 1, uint32_t max = 1000);
    bool coinFlip();
}
