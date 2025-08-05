#pragma once
#include <string>

enum class OrderSide {
    BUY,
    SELL
};

inline std::string orderSideToString(OrderSide side) {
    return side == OrderSide::BUY ? "BUY" : "SELL";
}
