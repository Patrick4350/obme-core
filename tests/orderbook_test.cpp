#include "../src/engine/OrderBook.h"
#include <cassert>
#include <iostream>
#include "../src/models/OrderType.h"
#include "../src/models/OrderSide.h"

void test_add_order() {
    OrderBook book;
    
    // Test Order constructor
    Order order(1, 100, "AAPL", OrderType::LIMIT, OrderSide::BUY, 150.0, 100);
    auto orderPtr = std::make_shared<Order>(order);
    
    // Test order validation
    assert(order.isValid());
    assert(!order.isPartiallyFilled());
    assert(!order.isFullyFilled());
    
    // Test order properties
    assert(order.orderId == 1);
    assert(order.clientId == 100);
    assert(order.symbol == "AAPL");
    assert(order.type == OrderType::LIMIT);
    assert(order.side == OrderSide::BUY);
    assert(order.price == 150.0);
    assert(order.quantity == 100);
    assert(order.remainingQty == 100);
    
    // Test adding to order book
    book.addOrder(orderPtr);
    
    // Test partial fill
    order.updateRemainingQty(30);
    assert(order.isPartiallyFilled());
    assert(!order.isFullyFilled());
    assert(order.getFilledQty() == 30);
    assert(order.remainingQty == 70);
    
    // Test full fill
    order.updateRemainingQty(70);
    assert(!order.isPartiallyFilled());
    assert(order.isFullyFilled());
    assert(order.getFilledQty() == 100);
    assert(order.remainingQty == 0);
    
    std::cout << "test_add_order passed\n";
}

void test_basic_limit_match() {
    OrderBook book;
    bool tradeHappened = false;
    book.setTradeCallback([&](const Order& buy, const Order& sell, double price, uint32_t qty) {
        tradeHappened = true;
        assert(buy.side == OrderSide::BUY);
        assert(sell.side == OrderSide::SELL);
        assert(price == 100.0);
        assert(qty == 10);
    });
    auto buy = std::make_shared<Order>();
    buy->orderId = 1;
    buy->symbol = "AAPL";
    buy->type = OrderType::LIMIT;
    buy->side = OrderSide::BUY;
    buy->price = 100.0;
    buy->quantity = 10;
    buy->remainingQty = 10;
    buy->timestamp = std::chrono::system_clock::now();
    book.addOrder(buy);
    auto sell = std::make_shared<Order>();
    sell->orderId = 2;
    sell->symbol = "AAPL";
    sell->type = OrderType::LIMIT;
    sell->side = OrderSide::SELL;
    sell->price = 100.0;
    sell->quantity = 10;
    sell->remainingQty = 10;
    sell->timestamp = std::chrono::system_clock::now();
    book.addOrder(sell);
    assert(tradeHappened);
    std::cout << "test_basic_limit_match passed\n";
}

void test_order_constructors() {
    // Test default constructor
    Order defaultOrder;
    assert(defaultOrder.orderId == 0);
    assert(defaultOrder.quantity == 0);
    assert(defaultOrder.remainingQty == 0);
    assert(!defaultOrder.isValid()); // Should be invalid with zero values
    
    // Test parameterized constructor
    Order order(123, 456, "MSFT", OrderType::MARKET, OrderSide::SELL, 250.75, 50);
    assert(order.orderId == 123);
    assert(order.clientId == 456);
    assert(order.symbol == "MSFT");
    assert(order.type == OrderType::MARKET);
    assert(order.side == OrderSide::SELL);
    assert(order.price == 250.75);
    assert(order.quantity == 50);
    assert(order.remainingQty == 50);
    assert(order.isValid());
    
    // Test copy constructor
    Order copied(order);
    assert(copied.orderId == order.orderId);
    assert(copied.symbol == order.symbol);
    assert(copied.type == order.type);
    assert(copied == order); // Test equality operator
    
    std::cout << "test_order_constructors passed\n";
}

void test_order_matching() {
    // Create buy and sell orders that can match
    Order buyOrder(1, 100, "GOOGL", OrderType::LIMIT, OrderSide::BUY, 2500.0, 10);
    Order sellOrder(2, 200, "GOOGL", OrderType::LIMIT, OrderSide::SELL, 2500.0, 15);
    
    // Test matching capability
    assert(buyOrder.canMatchWith(sellOrder));
    assert(sellOrder.canMatchWith(buyOrder));
    
    // Test execution price
    double execPrice = buyOrder.getExecutionPrice(sellOrder);
    assert(execPrice == 2500.0);
    
    // Test orders with different symbols shouldn't match
    Order differentSymbol(3, 300, "TSLA", OrderType::LIMIT, OrderSide::SELL, 800.0, 5);
    assert(!buyOrder.canMatchWith(differentSymbol));
    
    // Test same side orders shouldn't match
    Order sameSide(4, 400, "GOOGL", OrderType::LIMIT, OrderSide::BUY, 2450.0, 20);
    assert(!buyOrder.canMatchWith(sameSide));
    
    std::cout << "test_order_matching passed\n";
}

void test_order_lifecycle() {
    Order order(10, 500, "AMZN", OrderType::LIMIT, OrderSide::BUY, 3200.0, 100);
    
    // Initial state
    assert(!order.isPartiallyFilled());
    assert(!order.isFullyFilled());
    assert(order.getFilledQty() == 0);
    
    // Partial fill
    order.updateRemainingQty(30);
    assert(order.isPartiallyFilled());
    assert(!order.isFullyFilled());
    assert(order.getFilledQty() == 30);
    assert(order.remainingQty == 70);
    
    // Another partial fill
    order.updateRemainingQty(25);
    assert(order.isPartiallyFilled());
    assert(!order.isFullyFilled());
    assert(order.getFilledQty() == 55);
    assert(order.remainingQty == 45);
    
    // Complete fill
    order.updateRemainingQty(45);
    assert(!order.isPartiallyFilled());
    assert(order.isFullyFilled());
    assert(order.getFilledQty() == 100);
    assert(order.remainingQty == 0);
    
    // Test cancel functionality
    Order cancelOrder(20, 600, "NFLX", OrderType::LIMIT, OrderSide::SELL, 450.0, 50);
    cancelOrder.cancel();
    assert(cancelOrder.isFullyFilled()); // Cancelled orders appear as fully filled
    assert(cancelOrder.remainingQty == 0);
    
    std::cout << "test_order_lifecycle passed\n";
}

void test_order_string_representation() {
    Order order(999, 777, "META", OrderType::STOP_LIMIT, OrderSide::BUY, 300.50, 25, 305.0);
    
    std::string orderStr = order.toString();
    
    // Check that string contains key information
    assert(orderStr.find("999") != std::string::npos); // orderId
    assert(orderStr.find("777") != std::string::npos); // clientId
    assert(orderStr.find("META") != std::string::npos); // symbol
    assert(orderStr.find("STOP_LIMIT") != std::string::npos); // type
    assert(orderStr.find("BUY") != std::string::npos); // side
    assert(orderStr.find("300.50") != std::string::npos); // price
    assert(orderStr.find("25") != std::string::npos); // quantity
    assert(orderStr.find("305") != std::string::npos); // stopPrice
    
    std::cout << "test_order_string_representation passed\n";
}

int main() {
    std::cout << "=== Running OBME Core Order Tests ===\n";
    
    test_add_order();
    test_basic_limit_match();
    test_order_constructors();
    test_order_matching();
    test_order_lifecycle();
    test_order_string_representation();
    
    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
