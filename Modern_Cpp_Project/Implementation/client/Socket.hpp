// ===================================================================
// FILE: Socket.hpp
// DESCRIPTION: Abstract base class for Socket
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#ifndef SOCKET_HPP
#define SOCKET_HPP

// ===================================================================
// INCLUDES
// ===================================================================
#include <string>

// ===================================================================
// ABSTRACT SOCKET CLASS
// ===================================================================
/**
 * @brief Abstract base class for Socket
 * Defines common socket behaviors with pure virtual functions
 */
class Socket {
public:
    virtual ~Socket() = default;
    
    /**
     * @brief Wait for incoming connection (server-side)
     */
    virtual void waitForConnect() = 0;
    
    /**
     * @brief Connect to a server (client-side)
     */
    virtual void connect() = 0;
    
    /**
     * @brief Send data through the socket
     * @param message The message to send
     */
    virtual void send(const std::string& message) = 0;
    
    /**
     * @brief Receive data from the socket
     */
    virtual void receive() = 0;
    
    /**
     * @brief Shutdown the socket connection
     */
    virtual void shutdown() = 0;
};

#endif // SOCKET_HPP

// ===================================================================
// END OF FILE
// ===================================================================
