#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>
#include <iomanip>

enum class FeedType {
    FILE,
    NETWORK, 
    SIMULATION
};

using DataHandler = std::function<void(const std::string&)>;

class DataFeed {
public:
    DataFeed();
    ~DataFeed();
    
    // Connection management
    bool connect(const std::string& source);
    void disconnect();
    
    // Data streaming
    void start();
    void stop();
    
    // Configuration
    void setDataHandler(DataHandler handler);
    
    // Status
    bool isConnected() const;
    bool isRunning() const;
    std::string getSource() const;

private:
    void feedWorker();
    void processFileData();
    void processNetworkData();
    void processSimulationData();
    
    std::string generateSimulatedNetworkMessage(int messageId);
    std::string generateSimulatedMarketData(std::mt19937& rng, int dataId);
    
    std::string source_;
    FeedType feedType_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    std::thread worker_;
    DataHandler dataHandler_;
};
