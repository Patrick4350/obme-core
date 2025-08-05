#pragma once
#include "OrderBook.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "../io/Logger.h"

class Matcher {
public:
    Matcher(OrderBook& book, Logger& logger);
    void start();
    void stop();
    void submitOrder(const Order& order);
    uint64_t getProcessedOrders() const;
private:
    OrderBook& book_;
    Logger& logger_;
    std::queue<Order> orderQueue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread worker_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> processedOrders_{0};
    void run();
};
