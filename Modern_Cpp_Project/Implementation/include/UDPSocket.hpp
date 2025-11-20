// ===================================================================
// FILE: UDPSocket.hpp
// DESCRIPTION: UDP socket implementation using Asio library
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

// ===================================================================
// UDP SOCKET CLASS
// ===================================================================
class UDPSocket : public Socket {
private:
    // ===================================================================
    // MEMBER VARIABLES
    // ===================================================================
    asio::io_context io_context_;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remote_endpoint_;
    std::string local_ip_;
    int local_port_;
    std::string remote_ip_;
    int remote_port_;
    bool is_bound_;

public:
    // ===================================================================
    // CONSTRUCTORS & DESTRUCTOR
    // ===================================================================
    
    /**
     * @brief Constructor for client mode (sending/receiving)
     * @param remote_ip Remote IP address
     * @param remote_port Remote port number
     */
    UDPSocket(const std::string& remote_ip, int remote_port);
    
    /**
     * @brief Constructor for server mode (receiving)
     * @param local_ip Local IP address to bind
     * @param local_port Local port to bind
     * @param bind_immediately If true, bind immediately
     */
    UDPSocket(const std::string& local_ip, int local_port, bool bind_immediately);
    
    virtual ~UDPSocket();

    // ===================================================================
    // SOCKET INTERFACE IMPLEMENTATION
    // ===================================================================
    
    /**
     * @brief Wait for incoming connection (not applicable for UDP)
     * @return Always returns true for UDP
     */
    bool waitForConnect() override;
    
    /**
     * @brief Connect to remote endpoint (sets destination for sending)
     * @param ip Remote IP address
     * @param port Remote port number
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& ip, int port) override;
    
    /**
     * @brief Send data to remote endpoint
     * @param message Data to send
     * @return Number of bytes sent, or -1 on error
     */
    int send(const std::string& message) override;
    
    /**
     * @brief Receive data from any sender
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @return Number of bytes received, or -1 on error
     */
    int receive(char* buffer, int buffer_size) override;
    
    /**
     * @brief Set socket to non-blocking mode
     */
    void setNonBlocking(bool non_blocking) override;
    
    /**
     * @brief Shutdown and close socket
     */
    void shutdown() override;
};

// ===================================================================
// END OF FILE
// ===================================================================
