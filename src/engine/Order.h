#pragma once
#include <string>
#include <chrono>
#include "../models/OrderType.h"
#include "../models/OrderSide.h"

struct Order {
    // Core identifiers
    uint64_t orderId;
    uint64_t clientId;
    std::string symbol;
    
    // Order specifications
    OrderType type;
    OrderSide side;
    double price;           // For limit orders, ignored for market orders
    uint32_t quantity;
    uint32_t remainingQty;  // Tracks partial fills
    
    // Timing and lifecycle
    std::chrono::system_clock::time_point timestamp;
    std::chrono::system_clock::time_point lastModified;
    
    // For stop orders
    double stopPrice;       // Trigger price for stop orders
    
    // Constructors
    Order();
    Order(uint64_t id, uint64_t client, const std::string& sym, OrderType t, 
          OrderSide s, double p, uint32_t qty, double stop = 0.0);
    Order(const Order& other);
    Order& operator=(const Order& other);
    
    // Order lifecycle methods
    void updateRemainingQty(uint32_t filledQty);
    void cancel();
    uint32_t getFilledQty() const;
    
    // Matching and execution
    bool canMatchWith(const Order& other) const;
    double getExecutionPrice(const Order& other) const;
    
    // Utility methods
    std::string toString() const;
    bool operator==(const Order& other) const;
    bool operator<(const Order& other) const;
    
    // Validation and utility (inline implementations)
    bool isValid() const {
        return orderId > 0 && quantity > 0 && !symbol.empty();
    }
    
    bool isPartiallyFilled() const {
        return remainingQty < quantity && remainingQty > 0;
    }
    
    bool isFullyFilled() const {
        return remainingQty == 0;
    }
};
