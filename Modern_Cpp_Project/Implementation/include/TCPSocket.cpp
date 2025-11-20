// ===================================================================
// FILE: TCPSocket.cpp
// DESCRIPTION: TCP Socket implementation using Asio (derived from Socket)
// AUTHOR: Salah-Eldin Hassen
// DATE: November 20, 2025
// ===================================================================

#include "TCPSocket.hpp"

// ===================================================================
// CONSTRUCTOR & DESTRUCTOR
// ===================================================================

TCPSocket::TCPSocket(int port, const std::string& ip, bool is_server) 
    : socket_(io_context_), 
      acceptor_(nullptr), 
      ip_address(ip), 
      port(port), 
      is_server(is_server) {
    
    PRINT_DEBUG("TCPSocket constructor called");
    
    if (is_server) {
        // Server mode: create acceptor
        try {
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
            acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_context_, endpoint);
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

TCPSocket::~TCPSocket() {
    PRINT_DEBUG("TCPSocket destructor called");
    shutdown();
}

// ===================================================================
// SOCKET INTERFACE IMPLEMENTATION
// ===================================================================

bool TCPSocket::waitForConnect() {
    if (!is_server) {
        PRINT_ERROR("waitForConnect called on client socket");
        return false;
    }
    
    PRINT_DEBUG("Waiting for client connection...");
    
    try {
        acceptor_->accept(socket_);
        PRINT_DEBUG("Client connected");
        return true;
    } catch (const std::exception& e) {
        PRINT_ERROR("Failed to accept connection: " << e.what());
        return false;
    }
}

bool TCPSocket::connect(const std::string& ip, int port) {
    if (is_server) {
        PRINT_ERROR("connect called on server socket");
        return false;
    }
    
    PRINT_DEBUG("Attempting to connect to " << ip << ":" << port);
    
    try {
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(ip, std::to_string(port));
        
        asio::error_code ec;
        asio::connect(socket_, endpoints, ec);
        
        if (ec) {
            PRINT_ERROR("Connection failed: " << ec.message());
            return false;
        }
        
        PRINT_DEBUG("Connected to server successfully");
        return true;
    } catch (const std::exception& e) {
        PRINT_ERROR("Exception during connect: " << e.what());
        return false;
    }
}

int TCPSocket::send(const std::string& message) {
    PRINT_DEBUG("Sending message: " << message);
    
    try {
        asio::error_code ec;
        size_t sent = asio::write(socket_, asio::buffer(message.data(), message.size()), ec);
        
        if (ec) {
            PRINT_ERROR("Failed to send message: " << ec.message());
            return -1;
        } else {
            PRINT_DEBUG("Sent " << sent << " bytes");
            return static_cast<int>(sent);
        }
    } catch (const std::exception& e) {
        PRINT_ERROR("Exception during send: " << e.what());
        return -1;
    }
}

int TCPSocket::receive(char* buffer, int buffer_size) {
    PRINT_TRACE("Waiting to receive data...");
    
    try {
        asio::error_code ec;
        
        size_t received = socket_.read_some(asio::buffer(buffer, buffer_size), ec);
        
        // Check for would_block first (no data available in non-blocking mode)
        if (ec == asio::error::would_block || ec == asio::error::try_again) {
            return 0;  // No data available, but connection is still alive
        } else if (ec == asio::error::eof) {
            PRINT_DEBUG("Connection closed by peer");
            return -1;  // Connection closed, should break loop
        } else if (ec) {
            PRINT_ERROR("Failed to receive data: " << ec.message());
            return -1;
        } else if (received > 0) {
            PRINT_DEBUG("Received " << received << " bytes");
            return static_cast<int>(received);
        }
        return 0;
    } catch (const std::exception& e) {
        PRINT_ERROR("Exception during receive: " << e.what());
        return -1;
    }
}

void TCPSocket::shutdown() {
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

void TCPSocket::setNonBlocking(bool non_blocking) {
    try {
        socket_.non_blocking(non_blocking);
        PRINT_DEBUG("Socket set to " << (non_blocking ? "non-blocking" : "blocking") << " mode");
    } catch (const std::exception& e) {
        PRINT_ERROR("Failed to set non-blocking mode: " << e.what());
    }
}

// ===================================================================
// END OF FILE
// ===================================================================
