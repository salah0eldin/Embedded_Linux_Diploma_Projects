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
#include <asio.hpp>
#include <string>
#include <vector>
#include <QDebug>

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
        
        qDebug() << "TCPSocket constructor called";
        
        if (is_server) {
            // Server mode: create acceptor
            try {
                asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
                acceptor_ = new asio::ip::tcp::acceptor(io_context_, endpoint);
                acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));
                qDebug() << "Asio acceptor created on port " << port;
            } catch (const std::exception& e) {
                qCritical() << "Failed to create acceptor: " << e.what();
                throw;
            }
        } else {
            // Client mode: nothing to initialize until connect()
            qDebug() << "Client socket configured for " << ip_address << ":" << port;
        }
    }
    
    /**
     * @brief Destructor
     */
    ~TCPSocket() {
        qDebug() << "TCPSocket destructor called";
        shutdown();
        if (acceptor_) {
            delete acceptor_;
            acceptor_ = nullptr;
        }
    }
    
    /**
     * @brief Wait for incoming connection (server-side)
     * @return true if successful, false otherwise
     */
    bool waitForConnect() override {
        if (!is_server) {
            qCritical() << "waitForConnect called on client socket";
            return false;
        }
        
        qDebug() << "Waiting for client connection...";
        
        try {
            acceptor_->accept(socket_);
            qDebug() << "Client connected";
            return true;
        } catch (const std::exception& e) {
            qCritical() << "Failed to accept connection: " << e.what();
            return false;
        }
    }
    
    /**
     * @brief Connect to server (client-side)
     * @param ip Server IP address
     * @param port Server port number
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& ip, int port) override {
        if (is_server) {
            qCritical() << "connect called on server socket";
            return false;
        }
        
        qDebug() << "Attempting to connect to " << ip << ":" << port;
        
        try {
            asio::ip::tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(ip, std::to_string(port));
            
            asio::error_code ec;
            asio::connect(socket_, endpoints, ec);
            
            if (ec) {
                qCritical() << "Connection failed: " << ec.message();
                return false;
            }
            
            qDebug() << "Connected to server successfully";
            return true;
        } catch (const std::exception& e) {
            qCritical() << "Exception during connect: " << e.what();
            return false;
        }
    }
    
    /**
     * @brief Send data through the socket
     * @return Number of bytes sent, or -1 on error
     */
    int send(const std::string& message) override {
        qDebug() << "Sending message: " << message;
        
        try {
            asio::error_code ec;
            size_t sent = asio::write(socket_, asio::buffer(message.data(), message.size()), ec);
            
            if (ec) {
                qCritical() << "Failed to send message: " << ec.message();
                return -1;
            } else {
                qDebug() << "Sent " << sent << " bytes";
                return static_cast<int>(sent);
            }
        } catch (const std::exception& e) {
            qCritical() << "Exception during send: " << e.what();
            return -1;
        }
    }
    
    /**
     * @brief Receive data from the socket
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @return Number of bytes received, or -1 on error
     */
    int receive(char* buffer, int buffer_size) override {
        qDebug() << "Waiting to receive data...";
        
        try {
            asio::error_code ec;
            
            size_t received = socket_.read_some(asio::buffer(buffer, buffer_size), ec);
            
            // Check for would_block first (no data available in non-blocking mode)
            if (ec == asio::error::would_block || ec == asio::error::try_again) {
                return 0;  // No data available, but connection is still alive
            } else if (ec == asio::error::eof) {
                qDebug() << "Connection closed by peer";
                return -1;  // Connection closed, should break loop
            } else if (ec) {
                qCritical() << "Failed to receive data: " << ec.message();
                return -1;
            } else if (received > 0) {
                qDebug() << "Received " << received << " bytes";
                return static_cast<int>(received);
            }
            return 0;
        } catch (const std::exception& e) {
            qCritical() << "Exception during receive: " << e.what();
            return -1;
        }
    }
    
    /**
     * @brief Set socket to non-blocking mode
     */
    void setNonBlocking(bool non_blocking) override {
        try {
            socket_.non_blocking(non_blocking);
            qDebug() << "Socket set to " << (non_blocking ? "non-blocking" : "blocking") << " mode";
        } catch (const std::exception& e) {
            qCritical() << "Failed to set non-blocking mode: " << e.what();
        }
    }
    
    /**
     * @brief Shutdown the socket connection
     */
    void shutdown() override {
        qDebug() << "Shutting down TCP socket...";
        
        if (socket_.is_open()) {
            try {
                asio::error_code ec;
                socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
                socket_.close(ec);
                qDebug() << "Socket closed";
            } catch (const std::exception& e) {
                qCritical() << "Exception during shutdown: " << e.what();
            }
        }
    }
    
};

#endif // TCPSOCKET_HPP

// ===================================================================
// END OF FILE
// ===================================================================
