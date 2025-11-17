
#pragma once

#include "Socket.hpp"
#include <string>

/**
 * @brief Abstract base class for Channel
 * Handles socket interactions through channelSocket member
 */
class Channel {
protected:
    Socket* channelSocket;  // Pointer to Socket object
    
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
