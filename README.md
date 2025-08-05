# OBME Core

Order Book Matching Engine (OBME) core logic and modules.

## Overview

OBME Core is a high-performance, low-latency order book matching engine written in C++ with comprehensive testing and stress testing capabilities. The system supports various order types, real-time matching, and provides detailed logging and performance metrics.

## Features

### Core Order Management
- **Order Types**: MARKET, LIMIT, STOP, STOP_LIMIT, CANCEL, MODIFY
- **Order Sides**: BUY, SELL
- **Order Lifecycle**: Creation, partial fills, complete fills, cancellation
- **Real-time Validation**: Price, quantity, and symbol validation
- **Matching Logic**: Price-time priority with sophisticated matching algorithms

### Order Book Functionality
- **Multi-Symbol Support**: Handle orders for different symbols simultaneously
- **Price-Level Management**: Efficient bid/ask price level organization
- **Trade Execution**: Automatic matching with trade callbacks
- **Performance Metrics**: Total trades, best bid/ask tracking

### Data Processing
- **JSON Parser**: Parse orders from JSON format
- **CSV Parser**: Parse orders from CSV format
- **Data Feed Integration**: File, network, and simulation data sources
- **Logging System**: Comprehensive event and trade logging

### Stress Testing & Performance
- **High-Frequency Simulation**: Generate thousands of orders per second
- **Market Stress Testing**: Simulate various market conditions
- **Concurrent Load Testing**: Multi-threaded order generation
- **Performance Analysis**: Detailed metrics and statistics

## Project Structure

```
obme-core/
├── src/                    # Core C++ source code
│   ├── engine/            # Order book and matching engine
│   │   ├── Order.h/cpp    # Order class implementation
│   │   ├── OrderBook.h/cpp # Order book implementation
│   │   ├── Matcher.h/cpp  # Order matching engine
│   │   └── Utils.h/cpp    # Utility functions
│   ├── io/                # Input/Output modules
│   │   ├── Logger.h/cpp   # Logging system
│   │   ├── DataFeed.h/cpp # Data feed management
│   │   └── OrderParser.h/cpp # Order parsing utilities
│   ├── models/            # Data models and enums
│   │   ├── OrderType.h    # Order type definitions
│   │   └── OrderSide.h    # Order side definitions
│   └── main.cpp           # Main application entry point
├── tests/                 # Unit and integration tests
│   ├── orderbook_test.cpp # Order book test suite
│   └── matcher_test.cpp   # Matcher test suite
├── scripts/               # Python utilities and stress tests
│   ├── stress_test.py     # Comprehensive stress testing
│   └── demo.py           # Demonstration script
├── data/                  # Sample data and logs
│   ├── sample_orders.json # Sample order data
│   └── logs.txt          # Application logs
├── build/                 # Build output directory
├── CMakeLists.txt        # CMake build configuration
├── Makefile              # Make build configuration
└── README.md             # This file
```

## Building the Project

### Prerequisites
- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10+ (recommended) or Make
- Python 3.7+ (for stress testing)

### Build Options

#### Using CMake (Recommended)
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

#### Using Make
```bash
make
```

#### Building Tests Only
```bash
g++ -std=c++17 -Wall -I./src -I./src/engine -I./src/io -I./src/models \
    ./tests/orderbook_test.cpp ./src/engine/Order.cpp ./src/engine/OrderBook.cpp \
    -o ./tests/orderbook_test.exe
```

## Usage

### Running the Main Application
```bash
./build/obme-core
```

### Running Tests
```bash
./tests/orderbook_test.exe
```

### Running Stress Tests
```bash
python scripts/stress_test.py
```

### Running Demo
```bash
python scripts/demo.py
```

## Order Format Examples

### JSON Format
```json
{
  "orderId": 12345,
  "clientId": 100,
  "symbol": "AAPL",
  "type": "LIMIT",
  "side": "BUY",
  "price": 150.50,
  "quantity": 100,
  "remainingQty": 100,
  "stopPrice": 0.0
}
```

### CSV Format
```csv
orderId,symbol,type,side,price,quantity,clientId,remainingQty,stopPrice
12345,AAPL,LIMIT,BUY,150.50,100,100,100,0.0
```

## API Reference

### Order Class
```cpp
// Constructors
Order();
Order(uint64_t id, uint64_t client, const std::string& sym, 
      OrderType t, OrderSide s, double p, uint32_t qty, double stop = 0.0);

// Core Methods
bool isValid() const;
bool isPartiallyFilled() const;
bool isFullyFilled() const;
void updateRemainingQty(uint32_t filledQty);
void cancel();
uint32_t getFilledQty() const;

// Matching
bool canMatchWith(const Order& other) const;
double getExecutionPrice(const Order& other) const;

// Utility
std::string toString() const;
```

### OrderBook Class
```cpp
void addOrder(OrderPtr order);
void cancelOrder(uint64_t orderId);
void setTradeCallback(TradeCallback cb);
double getBestBid() const;
double getBestAsk() const;
uint64_t getTotalTrades() const;
```

## Performance Characteristics

### Benchmarks (Example System)
- **Order Processing**: 100,000+ orders/second
- **Latency**: Sub-microsecond matching
- **Memory Usage**: Efficient price-level organization
- **Throughput**: Handles high-frequency trading scenarios

### Stress Test Scenarios
1. **Quick Test**: 100 orders in rapid succession
2. **High Frequency**: 1000+ orders/second sustained
3. **Market Stress**: Various order types and market conditions
4. **Concurrent Load**: Multi-threaded order submission

## Configuration

### Logger Configuration
```cpp
Logger logger("path/to/logfile.txt");
// Logs are automatically timestamped and thread-safe
```

### OrderBook Configuration
```cpp
OrderBook book;
book.setTradeCallback([](const Order& buy, const Order& sell, double price, uint32_t qty) {
    // Handle trade execution
    std::cout << "Trade: " << qty << " @ " << price << std::endl;
});
```

## Contributing

1. Follow C++17 standards
2. Include comprehensive tests for new features
3. Update documentation
4. Ensure thread safety for concurrent operations
5. Maintain performance benchmarks

## Testing

### Unit Tests
```bash
# Run all order tests
./tests/orderbook_test.exe

# Expected output:
# === Running OBME Core Order Tests ===
# test_add_order passed
# test_basic_limit_match passed
# test_order_constructors passed
# test_order_matching passed
# test_order_lifecycle passed
# test_order_string_representation passed
# === All tests passed! ===
```

### Stress Tests
```bash
# Interactive stress test menu
python scripts/stress_test.py

# Available scenarios:
# 1. Quick Test (100 orders)
# 2. High Frequency Simulation (5000+ orders)
# 3. Market Stress Test (various conditions)
# 4. Concurrent Load Test (multi-threaded)
```

## License

See `LICENSE` file for details.

## Dependencies

### C++ Dependencies
- Standard Library (C++17)
- `<chrono>` for timestamps
- `<mutex>` for thread safety
- `<memory>` for smart pointers

### Python Dependencies (Testing Only)
- Python 3.7+
- Standard library modules only (no external dependencies)

## Future Enhancements

- [ ] WebSocket API for real-time order submission
- [ ] Persistence layer for order recovery
- [ ] Advanced order types (iceberg, hidden)
- [ ] Market data distribution
- [ ] Risk management integration
- [ ] Performance profiling tools
