#pragma once
#include <string>

enum class OrderType {
    MARKET,     // Execute immediately at best available price
    LIMIT,      // Execute only at specified price or better
    STOP,       // Trigger when price reaches stop level
    STOP_LIMIT, // Combines stop and limit functionality
    CANCEL,     // Cancel existing order
    MODIFY      // Modify existing order
};

// Utility functions for interview discussions
inline std::string orderTypeToString(OrderType type) {
    switch(type) {
        case OrderType::MARKET: return "MARKET";
        case OrderType::LIMIT: return "LIMIT";
        case OrderType::STOP: return "STOP";
        case OrderType::STOP_LIMIT: return "STOP_LIMIT";
        case OrderType::CANCEL: return "CANCEL";
        case OrderType::MODIFY: return "MODIFY";
        default: return "UNKNOWN";
    }
}
