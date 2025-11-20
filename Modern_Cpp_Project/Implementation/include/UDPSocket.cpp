// ===================================================================
// FILE: UDPSocket.cpp
// DESCRIPTION: UDP socket implementation using Asio library
// AUTHOR: Salah-Eldin Hassen
// DATE: November 20, 2025
// ===================================================================

#include "UDPSocket.hpp"

// ===================================================================
// CONSTRUCTORS & DESTRUCTOR
// ===================================================================

UDPSocket::UDPSocket(const std::string& remote_ip, int remote_port)
    : socket_(io_context_, asio::ip::udp::v4()),
      remote_ip_(remote_ip), 
      remote_port_(remote_port), 
      local_port_(0), 
      is_bound_(false) {
    PRINT_DEBUG("[UDPSocket] Created UDP socket for communicating with " + remote_ip + ":" + std::to_string(remote_port));
}

UDPSocket::UDPSocket(const std::string& local_ip, int local_port, bool bind_immediately)
    : socket_(io_context_, asio::ip::udp::v4()),
      local_ip_(local_ip), 
      local_port_(local_port), 
      remote_port_(0), 
      is_bound_(false) {
    try {
        if (bind_immediately) {
            asio::ip::udp::endpoint endpoint(asio::ip::address::from_string(local_ip), local_port);
            socket_.bind(endpoint);
            is_bound_ = true;
            PRINT_DEBUG("[UDPSocket] Bound UDP socket to " + local_ip + ":" + std::to_string(local_port));
        }
    } catch (const std::exception& e) {
        PRINT_ERROR("[UDPSocket] Failed to bind socket: " + std::string(e.what()));
        throw;
    }
}

UDPSocket::~UDPSocket() {
    if (socket_.is_open()) {
        PRINT_DEBUG("[UDPSocket] Closing UDP socket");
        socket_.close();
    }
}

// ===================================================================
// SOCKET INTERFACE IMPLEMENTATION
// ===================================================================

bool UDPSocket::waitForConnect() {
    PRINT_DEBUG("[UDPSocket] waitForConnect called (no-op for UDP)");
    return true;
}

bool UDPSocket::connect(const std::string& ip, int port) {
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

int UDPSocket::send(const std::string& message) {
    try {
        if (!socket_.is_open()) {
            PRINT_ERROR("[UDPSocket] Socket is not open");
            return -1;
        }
        
        asio::error_code ec;
        size_t bytes_sent = socket_.send_to(asio::buffer(message), remote_endpoint_, 0, ec);
        
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

int UDPSocket::receive(char* buffer, int buffer_size) {
    try {
        if (!socket_.is_open()) {
            PRINT_ERROR("[UDPSocket] Socket is not open");
            return -1;
        }
        
        asio::ip::udp::endpoint sender_endpoint;
        asio::error_code ec;
        
        size_t bytes_received = socket_.receive_from(asio::buffer(buffer, buffer_size), sender_endpoint, 0, ec);
        
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

void UDPSocket::shutdown() {
    try {
        if (socket_.is_open()) {
            PRINT_DEBUG("[UDPSocket] Shutting down UDP socket");
            socket_.close();
        }
    } catch (const std::exception& e) {
        PRINT_ERROR("[UDPSocket] Shutdown exception: " + std::string(e.what()));
    }
}

void UDPSocket::setNonBlocking(bool non_blocking) {
    try {
        if (socket_.is_open()) {
            socket_.non_blocking(non_blocking);
            PRINT_DEBUG("[UDPSocket] Socket set to " + std::string(non_blocking ? "non-blocking" : "blocking") + " mode");
        }
    } catch (const std::exception& e) {
        PRINT_ERROR("[UDPSocket] Failed to set non-blocking mode: " + std::string(e.what()));
    }
}

// ===================================================================
// END OF FILE
// ===================================================================
