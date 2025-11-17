
#pragma once

#include "Channel.hpp"
#include <iostream>

/**
 * @brief Client Channel implementation
 * Implements client-specific behaviors
 */
class ClientChannel : public Channel {
public:
    /**
     * @brief Constructor
     * @param socket Pointer to a Socket object
     */
    explicit ClientChannel(Socket* socket) : Channel(socket) {
        std::cout << "[DEBUG] ClientChannel created" << std::endl;
    }
    
    /**
     * @brief Destructor
     */
    ~ClientChannel() {
        std::cout << "[DEBUG] ClientChannel destructor called" << std::endl;
    }
    
    /**
     * @brief Start the client channel
     */
    void start() override {
        std::cout << "[INFO] Starting Client Channel..." << std::endl;
        if (channelSocket) {
            channelSocket->connect();
            std::cout << "[INFO] Client Channel started and connected to server" << std::endl;
        }
    }
    
    /**
     * @brief Stop the client channel
     */
    void stop() override {
        std::cout << "[INFO] Stopping Client Channel..." << std::endl;
        if (channelSocket) {
            channelSocket->shutdown();
            std::cout << "[INFO] Client Channel stopped" << std::endl;
        }
    }
    
    /**
     * @brief Send data through the client channel
     */
    void send(const std::string& message) override {
        std::cout << "[INFO] Client sending message..." << std::endl;
        if (channelSocket) {
            channelSocket->send(message);
        }
    }
    
    /**
     * @brief Receive data from the client channel
     */
    void receive() override {
        std::cout << "[INFO] Client receiving message..." << std::endl;
        if (channelSocket) {
            channelSocket->receive();
        }
    }
};

