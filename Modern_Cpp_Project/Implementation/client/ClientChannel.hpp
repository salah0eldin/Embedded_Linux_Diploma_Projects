// ===================================================================
// FILE: ClientChannel.hpp
// DESCRIPTION: Client Channel implementation (derived from Channel)
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include "Channel.hpp"
#include "config.h"

// ===================================================================
// CLIENT CHANNEL CLASS
// ===================================================================
/**
 * @brief Client Channel implementation
 * Implements client-specific behaviors
 */
class ClientChannel : public Channel {
// ===================================================================
// PUBLIC METHODS
// ===================================================================
public:
    /**
     * @brief Constructor
     * @param socket Pointer to a Socket object
     */
    explicit ClientChannel(Socket* socket) : Channel(socket) {
        PRINT_DEBUG("ClientChannel created");
    }
    
    /**
     * @brief Destructor
     */
    ~ClientChannel() {
        PRINT_DEBUG("ClientChannel destructor called");
    }
    
    /**
     * @brief Start the client channel
     */
    void start() override {
        PRINT_INFO("Starting Client Channel...");
        if (channelSocket) {
            channelSocket->connect();
            PRINT_INFO("Client Channel started and connected to server");
        }
    }
    
    /**
     * @brief Stop the client channel
     */
    void stop() override {
        PRINT_INFO("Stopping Client Channel...");
        if (channelSocket) {
            channelSocket->shutdown();
            PRINT_INFO("Client Channel stopped");
        }
    }
    
    /**
     * @brief Send data through the client channel
     */
    void send(const std::string& message) override {
        PRINT_INFO("Client sending message...");
        if (channelSocket) {
            channelSocket->send(message);
        }
    }
    
    /**
     * @brief Receive data from the client channel
     */
    void receive() override {
        PRINT_INFO("Client receiving message...");
        if (channelSocket) {
            channelSocket->receive();
        }
    }
};

// ===================================================================
// END OF FILE
// ===================================================================

