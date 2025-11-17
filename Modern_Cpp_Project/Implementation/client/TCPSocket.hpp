// ===================================================================
// FILE: TCPSocket.hpp
// DESCRIPTION: TCP Socket implementation using Asio (derived from Socket)
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

// ===================================================================
// INCLUDES
// ===================================================================
#include "Socket.hpp"
#include "config.h"
#include <asio.hpp>
#include <string>
#include <vector>

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
    asio::ip::tcp::acceptor* acceptor_;
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
    TCPSocket(int port, const std::string& ip = "127.0.0.1", bool is_server = false) 
        : socket_(io_context_), acceptor_(nullptr), ip_address(ip), port(port), is_server(is_server) {
        
        PRINT_DEBUG("TCPSocket constructor called");
        
        if (is_server) {
            // Server mode: create acceptor
            try {
                asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
                acceptor_ = new asio::ip::tcp::acceptor(io_context_, endpoint);
                acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));
                PRINT_DEBUG("Asio acceptor created on port " << port);
            } catch (const std::exception& e) {
                PRINT_ERROR("Failed to create acceptor: " << e.what());
                throw;
            }
        } else {
            // Client mode: nothing to initialize until connect()
            PRINT_DEBUG("Client socket configured for " << ip_address << ":" << port);
        }
    }
    
    /**
     * @brief Destructor
     */
    ~TCPSocket() {
        PRINT_DEBUG("TCPSocket destructor called");
        shutdown();
        if (acceptor_) {
            delete acceptor_;
            acceptor_ = nullptr;
        }
    }
    
    /**
     * @brief Wait for incoming connection (server-side)
     */
    void waitForConnect() override {
        if (!is_server) {
            PRINT_ERROR("waitForConnect called on client socket");
            return;
        }
        
        PRINT_DEBUG("Waiting for client connection...");
        
        try {
            acceptor_->accept(socket_);
            PRINT_DEBUG("Client connected");
        } catch (const std::exception& e) {
            PRINT_ERROR("Failed to accept connection: " << e.what());
            throw;
        }
    }
    
    /**
     * @brief Connect to server (client-side)
     */
    void connect() override {
        if (is_server) {
            PRINT_ERROR("connect called on server socket");
            return;
        }
        
        PRINT_DEBUG("Attempting to connect to " << ip_address << ":" << port);
        
        try {
            asio::ip::tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(ip_address, std::to_string(port));
            
            asio::error_code ec;
            asio::connect(socket_, endpoints, ec);
            
            if (ec) {
                PRINT_ERROR("Connection failed: " << ec.message());
                throw std::runtime_error("Connection failed: " + ec.message());
            }
            
            PRINT_DEBUG("Connected to server successfully");
        } catch (const std::exception& e) {
            PRINT_ERROR("Exception during connect: " << e.what());
            throw;
        }
    }
    
    /**
     * @brief Send data through the socket
     */
    void send(const std::string& message) override {
        PRINT_DEBUG("Sending message: " << message);
        
        try {
            asio::error_code ec;
            size_t sent = asio::write(socket_, asio::buffer(message.data(), message.size()), ec);
            
            if (ec) {
                PRINT_ERROR("Failed to send message: " << ec.message());
            } else {
                PRINT_DEBUG("Sent " << sent << " bytes");
            }
        } catch (const std::exception& e) {
            PRINT_ERROR("Exception during send: " << e.what());
        }
    }
    
    /**
     * @brief Receive data from the socket
     */
    void receive() override {
        PRINT_DEBUG("Waiting to receive data...");
        
        try {
            std::vector<char> buffer(1024);
            asio::error_code ec;
            
            size_t received = socket_.read_some(asio::buffer(buffer), ec);
            
            if (ec == asio::error::eof) {
                PRINT_DEBUG("Connection closed by peer");
            } else if (ec) {
                PRINT_ERROR("Failed to receive data: " << ec.message());
            } else if (received > 0) {
                std::string data(buffer.data(), received);
                PRINT_DEBUG("Received " << received << " bytes: " << data);
            }
        } catch (const std::exception& e) {
            PRINT_ERROR("Exception during receive: " << e.what());
        }
    }
    
    /**
     * @brief Shutdown the socket connection
     */
    void shutdown() override {
        PRINT_DEBUG("Shutting down TCP socket...");
        
        if (socket_.is_open()) {
            try {
                asio::error_code ec;
                socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
                socket_.close(ec);
                PRINT_DEBUG("Socket closed");
            } catch (const std::exception& e) {
                PRINT_ERROR("Exception during shutdown: " << e.what());
            }
        }
    }
};

#endif // TCPSOCKET_HPP

// ===================================================================
// END OF FILE
// ===================================================================
