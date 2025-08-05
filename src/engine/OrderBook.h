#pragma once
#include "Order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>
#include <atomic>

class Logger;

class OrderBook {
public:
    using OrderPtr = std::shared_ptr<Order>;
    using TradeCallback = std::function<void(const Order&, const Order&, double, uint32_t)>;

    OrderBook();
    void addOrder(OrderPtr order);
    void cancelOrder(uint64_t orderId);
    void setTradeCallback(TradeCallback cb);
    double getBestBid() const;
    double getBestAsk() const;
    uint64_t getTotalTrades() const;

private:
    // Price -> queue of orders (FIFO for price-time priority)
    std::map<double, std::deque<OrderPtr>, std::greater<double>> bids_;
    std::map<double, std::deque<OrderPtr>, std::less<double>> asks_;
    std::unordered_map<uint64_t, OrderPtr> orderMap_;
    mutable std::mutex mtx_;
    TradeCallback tradeCb_;
    std::atomic<uint64_t> totalTrades_{0};
    void match(OrderPtr order);
    void executeTrade(OrderPtr buy, OrderPtr sell, double price, uint32_t qty);
};
