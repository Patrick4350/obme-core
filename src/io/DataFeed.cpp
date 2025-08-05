#include "DataFeed.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

DataFeed::DataFeed() : connected_(false), running_(false) {}

DataFeed::~DataFeed() {
    disconnect();
}

bool DataFeed::connect(const std::string& source) {
    if (connected_) {
        std::cout << "Already connected to data source" << std::endl;
        return true;
    }
    
    source_ = source;
    
    // Simulate connection process
    std::cout << "Connecting to data source: " << source << std::endl;
    
    // Check if source is a file
    if (source.find(".json") != std::string::npos || 
        source.find(".csv") != std::string::npos ||
        source.find(".txt") != std::string::npos) {
        
        std::ifstream file(source);
        if (!file.is_open()) {
            std::cout << "Failed to open file: " << source << std::endl;
            return false;
        }
        file.close();
        
        feedType_ = FeedType::FILE;
        std::cout << "Connected to file data feed: " << source << std::endl;
    }
    else if (source.find("tcp://") == 0 || source.find("ws://") == 0) {
        // Simulate network connection
        feedType_ = FeedType::NETWORK;
        std::cout << "Connected to network data feed: " << source << std::endl;
    }
    else if (source == "simulation" || source == "random") {
        feedType_ = FeedType::SIMULATION;
        std::cout << "Connected to simulation data feed" << std::endl;
    }
    else {
        std::cout << "Unknown data source format: " << source << std::endl;
        return false;
    }
    
    connected_ = true;
    return true;
}

void DataFeed::disconnect() {
    if (!connected_) return;
    
    stop();
    connected_ = false;
    std::cout << "Disconnected from data source: " << source_ << std::endl;
}

void DataFeed::start() {
    if (!connected_) {
        std::cout << "Cannot start: not connected to data source" << std::endl;
        return;
    }
    
    if (running_) {
        std::cout << "Data feed already running" << std::endl;
        return;
    }
    
    running_ = true;
    worker_ = std::thread(&DataFeed::feedWorker, this);
    std::cout << "Data feed started" << std::endl;
}

void DataFeed::stop() {
    if (!running_) return;
    
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
    std::cout << "Data feed stopped" << std::endl;
}

void DataFeed::setDataHandler(DataHandler handler) {
    dataHandler_ = handler;
}

void DataFeed::feedWorker() {
    switch (feedType_) {
        case FeedType::FILE:
            processFileData();
            break;
        case FeedType::NETWORK:
            processNetworkData();
            break;
        case FeedType::SIMULATION:
            processSimulationData();
            break;
    }
}

void DataFeed::processFileData() {
    std::ifstream file(source_);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << source_ << std::endl;
        return;
    }
    
    std::string line;
    int lineCount = 0;
    
    while (running_ && std::getline(file, line)) {
        if (!line.empty() && dataHandler_) {
            dataHandler_(line);
        }
        
        lineCount++;
        if (lineCount % 1000 == 0) {
            std::cout << "Processed " << lineCount << " lines from file" << std::endl;
        }
        
        // Small delay to simulate real-time data feed
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    file.close();
    std::cout << "Finished processing file data. Total lines: " << lineCount << std::endl;
}

void DataFeed::processNetworkData() {
    // Simulate network data reception
    int messageCount = 0;
    
    while (running_) {
        // Simulate receiving network messages
        if (dataHandler_) {
            std::string simulatedMessage = generateSimulatedNetworkMessage(messageCount);
            dataHandler_(simulatedMessage);
        }
        
        messageCount++;
        if (messageCount % 100 == 0) {
            std::cout << "Received " << messageCount << " network messages" << std::endl;
        }
        
        // Simulate network latency
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void DataFeed::processSimulationData() {
    int dataCount = 0;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    
    while (running_) {
        if (dataHandler_) {
            std::string simulatedData = generateSimulatedMarketData(rng, dataCount);
            dataHandler_(simulatedData);
        }
        
        dataCount++;
        if (dataCount % 50 == 0) {
            std::cout << "Generated " << dataCount << " simulated data points" << std::endl;
        }
        
        // Simulate market data frequency
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

std::string DataFeed::generateSimulatedNetworkMessage(int messageId) {
    std::ostringstream oss;
    oss << "{\"messageId\":" << messageId 
        << ",\"type\":\"market_data\""
        << ",\"timestamp\":" << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count()
        << "}";
    return oss.str();
}

std::string DataFeed::generateSimulatedMarketData(std::mt19937& rng, int dataId) {
    std::uniform_real_distribution<> priceDist(99.0, 101.0);
    std::uniform_int_distribution<> qtyDist(1, 1000);
    std::uniform_int_distribution<> sideDist(0, 1);
    
    std::ostringstream oss;
    oss << "{\"dataId\":" << dataId
        << ",\"symbol\":\"AAPL\""
        << ",\"price\":" << std::fixed << std::setprecision(2) << priceDist(rng)
        << ",\"quantity\":" << qtyDist(rng)
        << ",\"side\":\"" << (sideDist(rng) ? "BUY" : "SELL") << "\""
        << ",\"timestamp\":" << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count()
        << "}";
    return oss.str();
}

bool DataFeed::isConnected() const {
    return connected_;
}

bool DataFeed::isRunning() const {
    return running_;
}

std::string DataFeed::getSource() const {
    return source_;
}
