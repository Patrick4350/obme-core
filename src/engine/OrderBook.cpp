#include "OrderBook.h"
#include <algorithm>
#include <cassert>

OrderBook::OrderBook() {}

void OrderBook::addOrder(OrderPtr order) {
    if (!order || !order->isValid()) return;
    std::lock_guard<std::mutex> lock(mtx_);
    orderMap_[order->orderId] = order;
    if (order->side == OrderSide::BUY) {
        match(order);
        if (order->remainingQty > 0) {
            bids_[order->price].push_back(order);
        }
    } else {
        match(order);
        if (order->remainingQty > 0) {
            asks_[order->price].push_back(order);
        }
    }
}

void OrderBook::cancelOrder(uint64_t orderId) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = orderMap_.find(orderId);
    if (it == orderMap_.end()) return;
    auto order = it->second;
    order->remainingQty = 0;
    // Remove from price level
    if (order->side == OrderSide::BUY) {
        auto pit = bids_.find(order->price);
        if (pit != bids_.end()) {
            auto& dq = pit->second;
            dq.erase(std::remove_if(dq.begin(), dq.end(), [orderId](const OrderPtr& o){ return o->orderId == orderId; }), dq.end());
            if (dq.empty()) bids_.erase(pit);
        }
    } else {
        auto pit = asks_.find(order->price);
        if (pit != asks_.end()) {
            auto& dq = pit->second;
            dq.erase(std::remove_if(dq.begin(), dq.end(), [orderId](const OrderPtr& o){ return o->orderId == orderId; }), dq.end());
            if (dq.empty()) asks_.erase(pit);
        }
    }
    orderMap_.erase(it);
}

void OrderBook::setTradeCallback(TradeCallback cb) {
    tradeCb_ = cb;
}

double OrderBook::getBestBid() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return bids_.empty() ? 0.0 : bids_.begin()->first;
}

double OrderBook::getBestAsk() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return asks_.empty() ? 0.0 : asks_.begin()->first;
}

uint64_t OrderBook::getTotalTrades() const {
    return totalTrades_.load();
}

void OrderBook::match(OrderPtr order) {
    if (order->side == OrderSide::BUY) {
        // Match buy order against asks (sell orders)
        while (order->remainingQty > 0 && !asks_.empty()) {
            auto it = asks_.begin();
            double price = it->first;
            if (order->price < price) {
                break; // Buy price too low to match
            }
            auto& dq = it->second;
            while (order->remainingQty > 0 && !dq.empty()) {
                auto matchOrder = dq.front();
                uint32_t fillQty = std::min(order->remainingQty, matchOrder->remainingQty);
                executeTrade(order, matchOrder, price, fillQty);
                if (matchOrder->remainingQty == 0) dq.pop_front();
            }
            if (dq.empty()) asks_.erase(it);
        }
    } else {
        // Match sell order against bids (buy orders)
        while (order->remainingQty > 0 && !bids_.empty()) {
            auto it = bids_.begin();
            double price = it->first;
            if (order->price > price) {
                break; // Sell price too high to match
            }
            auto& dq = it->second;
            while (order->remainingQty > 0 && !dq.empty()) {
                auto matchOrder = dq.front();
                uint32_t fillQty = std::min(order->remainingQty, matchOrder->remainingQty);
                executeTrade(matchOrder, order, price, fillQty);
                if (matchOrder->remainingQty == 0) dq.pop_front();
            }
            if (dq.empty()) bids_.erase(it);
        }
    }
}

void OrderBook::executeTrade(OrderPtr buy, OrderPtr sell, double price, uint32_t qty) {
    buy->remainingQty -= qty;
    sell->remainingQty -= qty;
    totalTrades_++;
    if (tradeCb_) tradeCb_(*buy, *sell, price, qty);
}
