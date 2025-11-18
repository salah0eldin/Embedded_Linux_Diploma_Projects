// ===================================================================
// FILE: Socket.hpp
// DESCRIPTION: Abstract base class for Socket
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

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
     * @return true if successful, false otherwise
     */
    virtual bool waitForConnect() = 0;
    
    /**
     * @brief Connect to a server (client-side)
     * @param ip Server IP address
     * @param port Server port number
     * @return true if successful, false otherwise
     */
    virtual bool connect(const std::string& ip, int port) = 0;
    
    /**
     * @brief Send data through the socket
     * @param message The message to send
     * @return Number of bytes sent, or -1 on error
     */
    virtual int send(const std::string& message) = 0;
    
    /**
     * @brief Receive data from the socket
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @return Number of bytes received, or -1 on error
     */
    virtual int receive(char* buffer, int buffer_size) = 0;
    
    /**
     * @brief Set socket to non-blocking mode
     */
    virtual void setNonBlocking(bool non_blocking) = 0;
    
    /**
     * @brief Shutdown the socket connection
     */
    virtual void shutdown() = 0;
};

// ===================================================================
// END OF FILE
// ===================================================================
