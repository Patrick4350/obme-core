#pragma once
#include <string>
#include <vector>
#include "../engine/Order.h"

class OrderParser {
public:
    OrderParser();
    
    // Main parsing function
    Order parse(const std::string& input);
    
    // Format-specific parsers
    Order parseJson(const std::string& json);
    Order parseCsv(const std::string& csv);
    Order parsePipeDelimited(const std::string& input);

private:
    // Utility functions
    OrderType stringToOrderType(const std::string& str);
    OrderSide stringToOrderSide(const std::string& str);
    
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    std::string toUpper(const std::string& str);
    
    // JSON parsing helpers
    std::string extractJsonString(const std::string& json, const std::string& key);
    
    template<typename T>
    T extractJsonValue(const std::string& json, const std::string& key, T defaultValue = T{});
};
