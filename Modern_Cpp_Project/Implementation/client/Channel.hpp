// ===================================================================
// FILE: Channel.hpp
// DESCRIPTION: Abstract base class for Channel
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include "Socket.hpp"
#include <string>

// ===================================================================
// ABSTRACT CHANNEL CLASS
// ===================================================================
/**
 * @brief Abstract base class for Channel
 * Handles socket interactions through channelSocket member
 */
class Channel {
// ===================================================================
// PROTECTED MEMBERS
// ===================================================================
protected:
    Socket* channelSocket;  // Pointer to Socket object
    
// ===================================================================
// PUBLIC METHODS
// ===================================================================
public:
    /**
     * @brief Constructor
     * @param socket Pointer to a Socket object
     */
    explicit Channel(Socket* socket) : channelSocket(socket) {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Channel() = default;
    
    /**
     * @brief Start the channel
     */
    virtual void start() = 0;
    
    /**
     * @brief Stop the channel
     */
    virtual void stop() = 0;
    
    /**
     * @brief Send data through the channel
     * @param message The message to send
     */
    virtual void send(const std::string& message) = 0;
    
    /**
     * @brief Receive data from the channel
     */
    virtual void receive() = 0;
};

// ===================================================================
// END OF FILE
// ===================================================================
