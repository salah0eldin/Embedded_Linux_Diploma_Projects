// ===================================================================
// FILE: TCPSocket.hpp
// DESCRIPTION: TCP Socket implementation using Asio (derived from Socket)
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include "Socket.hpp"
#include "config.h"
#include <asio.hpp>
#include <string>
#include <memory>

// ===================================================================
// TCP SOCKET CLASS
// ===================================================================

/**
 * @brief TCP Socket implementation using Asio
 * Implements TCP-specific socket behaviors with Asio library
 */
class TCPSocket : public Socket {
// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    std::string ip_address;
    int port;
    bool is_server;
    
// ===================================================================
// PUBLIC METHODS
// ===================================================================
public:
    /**
     * @brief Constructor for TCPSocket
     * @param port Port number
     * @param ip IP address (default: "127.0.0.1")
     * @param is_server True if server socket, false if client
     */
    TCPSocket(int port, const std::string& ip = "127.0.0.1", bool is_server = false);
    
    /**
     * @brief Destructor
     */
    ~TCPSocket();
    
    /**
     * @brief Wait for incoming connection (server-side)
     * @return true if successful, false otherwise
     */
    bool waitForConnect() override;
    
    /**
     * @brief Connect to server (client-side)
     * @param ip Server IP address
     * @param port Server port number
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& ip, int port) override;
    
    /**
     * @brief Send data through the socket
     * @return Number of bytes sent, or -1 on error
     */
    int send(const std::string& message) override;
    
    /**
     * @brief Receive data from the socket
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @return Number of bytes received, or -1 on error
     */
    int receive(char* buffer, int buffer_size) override;
    
    /**
     * @brief Shutdown the socket connection
     */
    void shutdown() override;
        
    /**
     * @brief Set socket to non-blocking mode
     */
    void setNonBlocking(bool non_blocking) override;
};

// ===================================================================
// END OF FILE
// ===================================================================
