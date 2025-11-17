#ifndef SERVERCHANNEL_HPP
#define SERVERCHANNEL_HPP

#include "Channel.hpp"
#include <iostream>

/**
 * @brief Server Channel implementation
 * Implements server-specific behaviors
 */
class ServerChannel : public Channel {
public:
    /**
     * @brief Constructor
     * @param socket Pointer to a Socket object
     */
    explicit ServerChannel(Socket* socket) : Channel(socket) {
        std::cout << "[DEBUG] ServerChannel created" << std::endl;
    }
    
    /**
     * @brief Destructor
     */
    ~ServerChannel() {
        std::cout << "[DEBUG] ServerChannel destructor called" << std::endl;
    }
    
    /**
     * @brief Start the server channel
     */
    void start() override {
        std::cout << "[INFO] Starting Server Channel..." << std::endl;
        if (channelSocket) {
            channelSocket->waitForConnect();
            std::cout << "[INFO] Server Channel started and client connected" << std::endl;
        }
    }
    
    /**
     * @brief Stop the server channel
     */
    void stop() override {
        std::cout << "[INFO] Stopping Server Channel..." << std::endl;
        if (channelSocket) {
            channelSocket->shutdown();
            std::cout << "[INFO] Server Channel stopped" << std::endl;
        }
    }
    
    /**
     * @brief Send data through the server channel
     */
    void send(const std::string& message) override {
        std::cout << "[INFO] Server sending message..." << std::endl;
        if (channelSocket) {
            channelSocket->send(message);
        }
    }
    
    /**
     * @brief Receive data from the server channel
     */
    void receive() override {
        std::cout << "[INFO] Server receiving message..." << std::endl;
        if (channelSocket) {
            channelSocket->receive();
        }
    }
};

#endif // SERVERCHANNEL_HPP
