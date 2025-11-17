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
#include <iostream>
#include <string>
#include <memory>

// ===================================================================
// UDP SOCKET CLASS
// ===================================================================
class UDPSocket : public Socket {
private:
    // ===================================================================
    // MEMBER VARIABLES
    // ===================================================================
    asio::io_context io_context_;
    std::unique_ptr<asio::ip::udp::socket> socket_;
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
     * @brief Constructor for client mode (sending)
     * @param remote_ip Remote IP address
     * @param remote_port Remote port number
     */
    UDPSocket(const std::string& remote_ip, int remote_port)
        : remote_ip_(remote_ip), remote_port_(remote_port), 
          local_port_(0), is_bound_(false) {
        try {
            socket_ = std::make_unique<asio::ip::udp::socket>(io_context_, asio::ip::udp::v4());
            PRINT_DEBUG("[UDPSocket] Created UDP socket for sending to " + remote_ip + ":" + std::to_string(remote_port));
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Failed to create socket: " + std::string(e.what()));
            throw;
        }
    }
    
    /**
     * @brief Constructor for server mode (receiving)
     * @param local_ip Local IP address to bind
     * @param local_port Local port to bind
     * @param bind_immediately If true, bind immediately
     */
    UDPSocket(const std::string& local_ip, int local_port, bool bind_immediately)
        : local_ip_(local_ip), local_port_(local_port), 
          remote_port_(0), is_bound_(false) {
        try {
            socket_ = std::make_unique<asio::ip::udp::socket>(io_context_, asio::ip::udp::v4());
            
            if (bind_immediately) {
                asio::ip::udp::endpoint endpoint(
                    asio::ip::address::from_string(local_ip), 
                    local_port
                );
                socket_->bind(endpoint);
                is_bound_ = true;
                PRINT_DEBUG("[UDPSocket] Bound UDP socket to " + local_ip + ":" + std::to_string(local_port));
            }
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Failed to create/bind socket: " + std::string(e.what()));
            throw;
        }
    }
    
    virtual ~UDPSocket() {
        if (socket_ && socket_->is_open()) {
            PRINT_DEBUG("[UDPSocket] Closing UDP socket");
            socket_->close();
        }
    }

    // ===================================================================
    // SOCKET INTERFACE IMPLEMENTATION
    // ===================================================================
    
    /**
     * @brief Wait for incoming connection (not applicable for UDP)
     * @return Always returns true for UDP
     */
    bool waitForConnect() override {
        PRINT_DEBUG("[UDPSocket] waitForConnect called (no-op for UDP)");
        return true;
    }
    
    /**
     * @brief Connect to remote endpoint (sets destination for sending)
     * @param ip Remote IP address
     * @param port Remote port number
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& ip, int port) override {
        try {
            remote_ip_ = ip;
            remote_port_ = port;
            
            // Resolve the remote endpoint
            asio::ip::udp::resolver resolver(io_context_);
            asio::ip::udp::resolver::results_type endpoints = 
                resolver.resolve(asio::ip::udp::v4(), ip, std::to_string(port));
            
            if (endpoints.empty()) {
                PRINT_ERROR("[UDPSocket] Failed to resolve " + ip + ":" + std::to_string(port));
                return false;
            }
            
            remote_endpoint_ = *endpoints.begin();
            
            PRINT_INFO("[UDPSocket] Connected (set remote endpoint) to " + ip + ":" + std::to_string(port));
            return true;
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Connect failed: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Send data to remote endpoint
     * @param message Data to send
     * @return Number of bytes sent, or -1 on error
     */
    int send(const std::string& message) override {
        try {
            if (!socket_ || !socket_->is_open()) {
                PRINT_ERROR("[UDPSocket] Socket is not open");
                return -1;
            }
            
            asio::error_code ec;
            size_t bytes_sent = socket_->send_to(
                asio::buffer(message), 
                remote_endpoint_, 
                0, 
                ec
            );
            
            if (ec) {
                PRINT_ERROR("[UDPSocket] Send failed: " + ec.message());
                return -1;
            }
            
            PRINT_TRACE("[UDPSocket] Sent " + std::to_string(bytes_sent) + " bytes");
            return static_cast<int>(bytes_sent);
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Send exception: " + std::string(e.what()));
            return -1;
        }
    }
    
    /**
     * @brief Receive data from any sender
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @return Number of bytes received, or -1 on error
     */
    int receive(char* buffer, int buffer_size) override {
        try {
            if (!socket_ || !socket_->is_open()) {
                PRINT_ERROR("[UDPSocket] Socket is not open");
                return -1;
            }
            
            asio::ip::udp::endpoint sender_endpoint;
            asio::error_code ec;
            
            size_t bytes_received = socket_->receive_from(
                asio::buffer(buffer, buffer_size),
                sender_endpoint,
                0,
                ec
            );
            
            if (ec) {
                if (ec == asio::error::would_block || ec == asio::error::try_again) {
                    return 0; // No data available
                }
                PRINT_ERROR("[UDPSocket] Receive failed: " + ec.message());
                return -1;
            }
            
            // Update remote endpoint with sender's address
            remote_endpoint_ = sender_endpoint;
            
            PRINT_TRACE("[UDPSocket] Received " + std::to_string(bytes_received) + 
                       " bytes from " + sender_endpoint.address().to_string() + 
                       ":" + std::to_string(sender_endpoint.port()));
            
            return static_cast<int>(bytes_received);
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Receive exception: " + std::string(e.what()));
            return -1;
        }
    }
    
    /**
     * @brief Shutdown and close socket
     */
    void shutdown() override {
        try {
            if (socket_ && socket_->is_open()) {
                PRINT_DEBUG("[UDPSocket] Shutting down UDP socket");
                socket_->close();
            }
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Shutdown exception: " + std::string(e.what()));
        }
    }
    
    // ===================================================================
    // ADDITIONAL UDP-SPECIFIC METHODS
    // ===================================================================
    
    /**
     * @brief Bind socket to local address (for receiving)
     * @param ip Local IP address
     * @param port Local port
     * @return true if successful, false otherwise
     */
    bool bind(const std::string& ip, int port) {
        try {
            if (is_bound_) {
                PRINT_DEBUG("[UDPSocket] Socket already bound");
                return true;
            }
            
            asio::ip::udp::endpoint endpoint(
                asio::ip::address::from_string(ip), 
                port
            );
            socket_->bind(endpoint);
            is_bound_ = true;
            local_ip_ = ip;
            local_port_ = port;
            
            PRINT_INFO("[UDPSocket] Bound to " + ip + ":" + std::to_string(port));
            return true;
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Bind failed: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Set socket to non-blocking mode
     * @param non_blocking If true, set non-blocking
     */
    void setNonBlocking(bool non_blocking) {
        try {
            if (socket_ && socket_->is_open()) {
                socket_->non_blocking(non_blocking);
                PRINT_DEBUG("[UDPSocket] Set non-blocking mode: " + std::string(non_blocking ? "true" : "false"));
            }
        } catch (const std::exception& e) {
            PRINT_ERROR("[UDPSocket] Failed to set non-blocking: " + std::string(e.what()));
        }
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
