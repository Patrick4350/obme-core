// Entry point for OBME core
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include "engine/OrderBook.h"
#include "engine/Matcher.h"
#include "io/Logger.h"
#include "models/OrderType.h"
#include "models/OrderSide.h"

int main() {
    Logger logger("../data/logs.txt");
    OrderBook book;
    book.setTradeCallback([&logger](const Order& buy, const Order& sell, double price, uint32_t qty) {
        logger.logTrade("buy=" + std::to_string(buy.orderId) + ",sell=" + std::to_string(sell.orderId) + ",price=" + std::to_string(price) + ",qty=" + std::to_string(qty));
    });
    Matcher matcher(book, logger);
    matcher.start();

    const int numOrders = 10000;
    std::mt19937 rng(42);
    std::uniform_int_distribution<> sideDist(0, 1);
    std::uniform_real_distribution<> priceDist(99.0, 101.0);
    std::uniform_int_distribution<> qtyDist(1, 100);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= numOrders; ++i) {
        Order order;
        order.orderId = i;
        order.symbol = "AAPL";
        order.type = OrderType::LIMIT;
        order.side = sideDist(rng) == 0 ? OrderSide::BUY : OrderSide::SELL;
        order.price = priceDist(rng);
        order.quantity = qtyDist(rng);
        order.remainingQty = order.quantity;
        order.timestamp = std::chrono::system_clock::now();
        matcher.submitOrder(order);
    }
    matcher.stop();
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Processed " << matcher.getProcessedOrders() << " orders in " << ms << " ms (" << (matcher.getProcessedOrders() / ms * 1000) << "/sec)\n";
    std::cout << "Total trades: " << book.getTotalTrades() << std::endl;
    std::cout << "Best Bid: " << book.getBestBid() << ", Best Ask: " << book.getBestAsk() << std::endl;
    return 0;
}
