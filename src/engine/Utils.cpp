#include "Utils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

namespace Utils {

std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timePoint.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string formatPrice(double price) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << price;
    return oss.str();
}

std::string formatQuantity(uint32_t quantity) {
    std::ostringstream oss;
    oss << quantity;
    return oss.str();
}

double roundToTickSize(double price, double tickSize) {
    if (tickSize <= 0.0) return price;
    return std::round(price / tickSize) * tickSize;
}

bool isValidPrice(double price) {
    return price > 0.0 && std::isfinite(price);
}

bool isValidQuantity(uint32_t quantity) {
    return quantity > 0;
}

std::string generateOrderId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    
    return std::to_string(dis(gen));
}

double calculateNotionalValue(double price, uint32_t quantity) {
    return price * quantity;
}

double calculateWeightedAveragePrice(const std::vector<std::pair<double, uint32_t>>& fills) {
    if (fills.empty()) return 0.0;
    
    double totalValue = 0.0;
    uint32_t totalQuantity = 0;
    
    for (const auto& fill : fills) {
        totalValue += fill.first * fill.second;
        totalQuantity += fill.second;
    }
    
    return totalQuantity > 0 ? totalValue / totalQuantity : 0.0;
}

std::string toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool isWithinPercentage(double value1, double value2, double percentage) {
    if (value2 == 0.0) return value1 == 0.0;
    
    double diff = std::abs(value1 - value2);
    double threshold = std::abs(value2) * (percentage / 100.0);
    
    return diff <= threshold;
}

uint64_t getMicrosecondsSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64_t getNanosecondsSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

std::string getRandomSymbol() {
    static const std::vector<std::string> symbols = {
        "AAPL", "GOOGL", "MSFT", "TSLA", "AMZN", "META", "NVDA", "NFLX", "BABA", "CRM"
    };
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, symbols.size() - 1);
    
    return symbols[dis(gen)];
}

double getRandomPrice(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    
    return dis(gen);
}

uint32_t getRandomQuantity(uint32_t min, uint32_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(min, max);
    
    return dis(gen);
}

bool coinFlip() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 1);
    
    return dis(gen) == 1;
}

std::string formatBytes(uint64_t bytes) {
    const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblBytes = bytes;
    
    while (dblBytes >= 1024 && i < 4) {
        dblBytes /= 1024;
        i++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << dblBytes << " " << sizes[i];
    return oss.str();
}

std::string formatDuration(std::chrono::milliseconds duration) {
    auto ms = duration.count();
    
    if (ms < 1000) {
        return std::to_string(ms) + "ms";
    }
    
    auto seconds = ms / 1000;
    ms %= 1000;
    
    if (seconds < 60) {
        return std::to_string(seconds) + "s " + std::to_string(ms) + "ms";
    }
    
    auto minutes = seconds / 60;
    seconds %= 60;
    
    return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}

} // namespace Utils
