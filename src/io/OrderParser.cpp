#include "OrderParser.h"
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

OrderParser::OrderParser() {}

Order OrderParser::parse(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Empty input string");
    }
    
    // Try to parse as JSON first
    if (input.front() == '{' && input.back() == '}') {
        return parseJson(input);
    }
    
    // Try to parse as CSV
    if (input.find(',') != std::string::npos) {
        return parseCsv(input);
    }
    
    // Try to parse as pipe-delimited
    if (input.find('|') != std::string::npos) {
        return parsePipeDelimited(input);
    }
    
    throw std::invalid_argument("Unrecognized input format");
}

Order OrderParser::parseJson(const std::string& json) {
    Order order;
    
    try {
        // Simple JSON parsing (not using external library)
        // This is a basic implementation - in production, use a proper JSON library
        
        order.orderId = extractJsonValue<uint64_t>(json, "orderId");
        order.clientId = extractJsonValue<uint64_t>(json, "clientId", 0);
        order.symbol = extractJsonString(json, "symbol");
        
        std::string typeStr = extractJsonString(json, "type");
        order.type = stringToOrderType(typeStr);
        
        std::string sideStr = extractJsonString(json, "side");
        order.side = stringToOrderSide(sideStr);
        
        order.price = extractJsonValue<double>(json, "price", 0.0);
        order.quantity = extractJsonValue<uint32_t>(json, "quantity");
        order.remainingQty = extractJsonValue<uint32_t>(json, "remainingQty", order.quantity);
        order.stopPrice = extractJsonValue<double>(json, "stopPrice", 0.0);
        
        // Set timestamps
        order.timestamp = std::chrono::system_clock::now();
        order.lastModified = order.timestamp;
        
    } catch (const std::exception& e) {
        throw std::invalid_argument("JSON parsing error: " + std::string(e.what()));
    }
    
    if (!order.isValid()) {
        throw std::invalid_argument("Parsed order is invalid");
    }
    
    return order;
}

Order OrderParser::parseCsv(const std::string& csv) {
    std::vector<std::string> tokens = split(csv, ',');
    
    if (tokens.size() < 6) {
        throw std::invalid_argument("CSV format requires at least 6 fields: orderId,symbol,type,side,price,quantity");
    }
    
    Order order;
    
    try {
        order.orderId = std::stoull(trim(tokens[0]));
        order.symbol = trim(tokens[1]);
        order.type = stringToOrderType(trim(tokens[2]));
        order.side = stringToOrderSide(trim(tokens[3]));
        order.price = std::stod(trim(tokens[4]));
        order.quantity = std::stoul(trim(tokens[5]));
        
        // Optional fields
        if (tokens.size() > 6) order.clientId = std::stoull(trim(tokens[6]));
        if (tokens.size() > 7) order.remainingQty = std::stoul(trim(tokens[7]));
        else order.remainingQty = order.quantity;
        if (tokens.size() > 8) order.stopPrice = std::stod(trim(tokens[8]));
        
        order.timestamp = std::chrono::system_clock::now();
        order.lastModified = order.timestamp;
        
    } catch (const std::exception& e) {
        throw std::invalid_argument("CSV parsing error: " + std::string(e.what()));
    }
    
    if (!order.isValid()) {
        throw std::invalid_argument("Parsed order is invalid");
    }
    
    return order;
}

Order OrderParser::parsePipeDelimited(const std::string& input) {
    std::vector<std::string> tokens = split(input, '|');
    
    if (tokens.size() < 6) {
        throw std::invalid_argument("Pipe-delimited format requires at least 6 fields");
    }
    
    Order order;
    
    try {
        order.orderId = std::stoull(trim(tokens[0]));
        order.symbol = trim(tokens[1]);
        order.type = stringToOrderType(trim(tokens[2]));
        order.side = stringToOrderSide(trim(tokens[3]));
        order.price = std::stod(trim(tokens[4]));
        order.quantity = std::stoul(trim(tokens[5]));
        order.remainingQty = order.quantity;
        
        order.timestamp = std::chrono::system_clock::now();
        order.lastModified = order.timestamp;
        
    } catch (const std::exception& e) {
        throw std::invalid_argument("Pipe-delimited parsing error: " + std::string(e.what()));
    }
    
    if (!order.isValid()) {
        throw std::invalid_argument("Parsed order is invalid");
    }
    
    return order;
}

OrderType OrderParser::stringToOrderType(const std::string& str) {
    std::string upper = toUpper(str);
    
    if (upper == "MARKET") return OrderType::MARKET;
    if (upper == "LIMIT") return OrderType::LIMIT;
    if (upper == "STOP") return OrderType::STOP;
    if (upper == "STOP_LIMIT") return OrderType::STOP_LIMIT;
    if (upper == "CANCEL") return OrderType::CANCEL;
    if (upper == "MODIFY") return OrderType::MODIFY;
    
    throw std::invalid_argument("Unknown order type: " + str);
}

OrderSide OrderParser::stringToOrderSide(const std::string& str) {
    std::string upper = toUpper(str);
    
    if (upper == "BUY") return OrderSide::BUY;
    if (upper == "SELL") return OrderSide::SELL;
    
    throw std::invalid_argument("Unknown order side: " + str);
}

std::vector<std::string> OrderParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string OrderParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string OrderParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string OrderParser::extractJsonString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        throw std::invalid_argument("Key not found: " + key);
    }
    
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) {
        throw std::invalid_argument("Invalid JSON format for key: " + key);
    }
    
    size_t valueStart = json.find('"', colonPos);
    if (valueStart == std::string::npos) {
        throw std::invalid_argument("String value not found for key: " + key);
    }
    
    size_t valueEnd = json.find('"', valueStart + 1);
    if (valueEnd == std::string::npos) {
        throw std::invalid_argument("Unterminated string value for key: " + key);
    }
    
    return json.substr(valueStart + 1, valueEnd - valueStart - 1);
}

template<typename T>
T OrderParser::extractJsonValue(const std::string& json, const std::string& key, T defaultValue) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == std::string::npos) {
        return defaultValue;
    }
    
    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) {
        return defaultValue;
    }
    
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && std::isspace(json[valueStart])) {
        valueStart++;
    }
    
    size_t valueEnd = valueStart;
    while (valueEnd < json.length() && 
           json[valueEnd] != ',' && 
           json[valueEnd] != '}' && 
           !std::isspace(json[valueEnd])) {
        valueEnd++;
    }
    
    if (valueStart >= valueEnd) {
        return defaultValue;
    }
    
    std::string valueStr = json.substr(valueStart, valueEnd - valueStart);
    
    try {
        if constexpr (std::is_same_v<T, uint64_t>) {
            return std::stoull(valueStr);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return std::stoul(valueStr);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(valueStr);
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(valueStr);
        }
    } catch (const std::exception&) {
        return defaultValue;
    }
    
    return defaultValue;
}

// Explicit template instantiations
template uint64_t OrderParser::extractJsonValue<uint64_t>(const std::string&, const std::string&, uint64_t);
template uint32_t OrderParser::extractJsonValue<uint32_t>(const std::string&, const std::string&, uint32_t);
template double OrderParser::extractJsonValue<double>(const std::string&, const std::string&, double);
template int OrderParser::extractJsonValue<int>(const std::string&, const std::string&, int);
