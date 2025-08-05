#include "Order.h"
#include <sstream>
#include <iomanip>

// Default constructor
Order::Order() 
    : orderId(0), clientId(0), symbol(""), type(OrderType::LIMIT), side(OrderSide::BUY),
      price(0.0), quantity(0), remainingQty(0), stopPrice(0.0) {
    timestamp = std::chrono::system_clock::now();
    lastModified = timestamp;
}

// Full constructor
Order::Order(uint64_t id, uint64_t client, const std::string& sym, OrderType t, 
             OrderSide s, double p, uint32_t qty, double stop)
    : orderId(id), clientId(client), symbol(sym), type(t), side(s), 
      price(p), quantity(qty), remainingQty(qty), stopPrice(stop) {
    timestamp = std::chrono::system_clock::now();
    lastModified = timestamp;
}

// Copy constructor
Order::Order(const Order& other) 
    : orderId(other.orderId), clientId(other.clientId), symbol(other.symbol),
      type(other.type), side(other.side), price(other.price), 
      quantity(other.quantity), remainingQty(other.remainingQty),
      timestamp(other.timestamp), lastModified(other.lastModified),
      stopPrice(other.stopPrice) {
}

// Assignment operator
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        orderId = other.orderId;
        clientId = other.clientId;
        symbol = other.symbol;
        type = other.type;
        side = other.side;
        price = other.price;
        quantity = other.quantity;
        remainingQty = other.remainingQty;
        timestamp = other.timestamp;
        lastModified = other.lastModified;
        stopPrice = other.stopPrice;
    }
    return *this;
}

// Update remaining quantity (for partial fills)
void Order::updateRemainingQty(uint32_t filledQty) {
    if (filledQty <= remainingQty) {
        remainingQty -= filledQty;
        lastModified = std::chrono::system_clock::now();
    }
}

// Cancel the order
void Order::cancel() {
    remainingQty = 0;
    lastModified = std::chrono::system_clock::now();
}

// Get filled quantity
uint32_t Order::getFilledQty() const {
    return quantity - remainingQty;
}

// Check if order can match with another order
bool Order::canMatchWith(const Order& other) const {
    // Same symbol check
    if (symbol != other.symbol) return false;
    
    // Opposite sides check
    if (side == other.side) return false;
    
    // Both orders must be valid
    if (!isValid() || !other.isValid()) return false;
    
    // Both orders must have remaining quantity
    if (remainingQty == 0 || other.remainingQty == 0) return false;
    
    // Price matching logic for limit orders
    if (type == OrderType::LIMIT && other.type == OrderType::LIMIT) {
        if (side == OrderSide::BUY) {
            // Buy order price must be >= sell order price
            return price >= other.price;
        } else {
            // Sell order price must be <= buy order price
            return price <= other.price;
        }
    }
    
    // Market orders can always match with any valid opposite order
    if (type == OrderType::MARKET || other.type == OrderType::MARKET) {
        return true;
    }
    
    return false;
}

// Get the execution price for matching with another order
double Order::getExecutionPrice(const Order& other) const {
    // For market orders, use the price of the limit order
    if (type == OrderType::MARKET && other.type == OrderType::LIMIT) {
        return other.price;
    }
    if (other.type == OrderType::MARKET && type == OrderType::LIMIT) {
        return price;
    }
    
    // For limit vs limit, use the price of the order that was placed first (timestamp)
    if (type == OrderType::LIMIT && other.type == OrderType::LIMIT) {
        return (timestamp < other.timestamp) ? price : other.price;
    }
    
    // Default case (shouldn't happen in normal flow)
    return std::max(price, other.price);
}

// Convert order to string representation
std::string Order::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Order[ID=" << orderId 
        << ", Client=" << clientId
        << ", Symbol=" << symbol
        << ", Type=" << orderTypeToString(type)
        << ", Side=" << orderSideToString(side)
        << ", Price=" << price
        << ", Qty=" << quantity
        << ", Remaining=" << remainingQty;
    
    if (type == OrderType::STOP || type == OrderType::STOP_LIMIT) {
        oss << ", StopPrice=" << stopPrice;
    }
    
    oss << "]";
    return oss.str();
}

// Equality operator
bool Order::operator==(const Order& other) const {
    return orderId == other.orderId;
}

// Less than operator (for sorting by timestamp)
bool Order::operator<(const Order& other) const {
    return timestamp < other.timestamp;
}
