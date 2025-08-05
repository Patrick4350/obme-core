#include "Matcher.h"

Matcher::Matcher(OrderBook& book, Logger& logger) : book_(book), logger_(logger) {}

void Matcher::start() {
    running_ = true;
    worker_ = std::thread(&Matcher::run, this);
}

void Matcher::stop() {
    running_ = false;
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
}

void Matcher::submitOrder(const Order& order) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        orderQueue_.push(order);
    }
    cv_.notify_one();
}

uint64_t Matcher::getProcessedOrders() const {
    return processedOrders_.load();
}

void Matcher::run() {
    while (running_) {
        Order order;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&]{ return !orderQueue_.empty() || !running_; });
            if (!running_ && orderQueue_.empty()) break;
            order = orderQueue_.front();
            orderQueue_.pop();
        }
        auto orderPtr = std::make_shared<Order>(order);
        book_.addOrder(orderPtr);
        processedOrders_++;
        logger_.log("Order processed: id=" + std::to_string(order.orderId));
    }
}
