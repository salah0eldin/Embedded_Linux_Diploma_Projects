#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

/**
 * @brief TCP Socket implementation
 * Implements TCP-specific socket behaviors
 */
class TCPSocket : public Socket {
private:
    int socket_fd;
    int client_fd;
    int port;
    std::string ip_address;
    bool is_server;
    struct sockaddr_in address;
    
public:
    /**
     * @brief Constructor for TCPSocket
     * @param port Port number
     * @param ip IP address (default: "127.0.0.1")
     * @param is_server True if server socket, false if client
     */
    TCPSocket(int port, const std::string& ip = "127.0.0.1", bool is_server = false) 
        : socket_fd(-1), client_fd(-1), port(port), ip_address(ip), is_server(is_server) {
        std::cout << "[DEBUG] TCPSocket constructor called" << std::endl;
        
        // Create socket
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cerr << "[ERROR] Failed to create socket" << std::endl;
            throw std::runtime_error("Socket creation failed");
        }
        std::cout << "[DEBUG] TCP Socket created (fd: " << socket_fd << ")" << std::endl;
        
        // Setup address structure
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        
        if (is_server) {
            address.sin_addr.s_addr = INADDR_ANY;
            
            // Set socket options
            int opt = 1;
            if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                std::cerr << "[ERROR] Failed to set socket options" << std::endl;
            }
            std::cout << "[DEBUG] Socket options set (SO_REUSEADDR)" << std::endl;
            
            // Bind
            if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                std::cerr << "[ERROR] Failed to bind socket to port " << port << std::endl;
                close(socket_fd);
                throw std::runtime_error("Socket bind failed");
            }
            std::cout << "[DEBUG] Socket bound to port " << port << std::endl;
            
            // Listen
            if (listen(socket_fd, 3) < 0) {
                std::cerr << "[ERROR] Failed to listen on socket" << std::endl;
                close(socket_fd);
                throw std::runtime_error("Socket listen failed");
            }
            std::cout << "[DEBUG] Socket listening on port " << port << std::endl;
        } else {
            if (inet_pton(AF_INET, ip_address.c_str(), &address.sin_addr) <= 0) {
                std::cerr << "[ERROR] Invalid address" << std::endl;
                close(socket_fd);
                throw std::runtime_error("Invalid address");
            }
            std::cout << "[DEBUG] Client socket configured for " << ip_address << ":" << port << std::endl;
        }
    }
    
    /**
     * @brief Destructor
     */
    ~TCPSocket() {
        std::cout << "[DEBUG] TCPSocket destructor called" << std::endl;
        shutdown();
    }
    
    /**
     * @brief Wait for incoming connection (server-side)
     */
    void waitForConnect() override {
        if (!is_server) {
            std::cerr << "[ERROR] waitForConnect called on client socket" << std::endl;
            return;
        }
        
        std::cout << "[DEBUG] Waiting for client connection..." << std::endl;
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_len);
        
        if (client_fd < 0) {
            std::cerr << "[ERROR] Failed to accept client connection" << std::endl;
            throw std::runtime_error("Accept failed");
        }
        
        std::cout << "[DEBUG] Client connected (fd: " << client_fd << ")" << std::endl;
    }
    
    /**
     * @brief Connect to server (client-side)
     */
    void connect() override {
        if (is_server) {
            std::cerr << "[ERROR] connect called on server socket" << std::endl;
            return;
        }
        
        std::cout << "[DEBUG] Attempting to connect to " << ip_address << ":" << port << std::endl;
        if (::connect(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "[ERROR] Connection failed" << std::endl;
            throw std::runtime_error("Connection failed");
        }
        std::cout << "[DEBUG] Connected to server successfully" << std::endl;
    }
    
    /**
     * @brief Send data through the socket
     */
    void send(const std::string& message) override {
        int fd = is_server ? client_fd : socket_fd;
        std::cout << "[DEBUG] Sending message: " << message << std::endl;
        
        ssize_t sent = ::send(fd, message.c_str(), message.length(), 0);
        if (sent < 0) {
            std::cerr << "[ERROR] Failed to send message" << std::endl;
        } else {
            std::cout << "[DEBUG] Sent " << sent << " bytes" << std::endl;
        }
    }
    
    /**
     * @brief Receive data from the socket
     */
    void receive() override {
        int fd = is_server ? client_fd : socket_fd;
        char buffer[1024] = {0};
        
        std::cout << "[DEBUG] Waiting to receive data..." << std::endl;
        ssize_t received = recv(fd, buffer, sizeof(buffer) - 1, 0);
        
        if (received < 0) {
            std::cerr << "[ERROR] Failed to receive data" << std::endl;
        } else if (received == 0) {
            std::cout << "[DEBUG] Connection closed by peer" << std::endl;
        } else {
            buffer[received] = '\0';
            std::cout << "[DEBUG] Received " << received << " bytes: " << buffer << std::endl;
        }
    }
    
    /**
     * @brief Shutdown the socket connection
     */
    void shutdown() override {
        std::cout << "[DEBUG] Shutting down TCP socket..." << std::endl;
        
        if (client_fd >= 0) {
            std::cout << "[DEBUG] Closing client connection (fd: " << client_fd << ")" << std::endl;
            close(client_fd);
            client_fd = -1;
        }
        
        if (socket_fd >= 0) {
            std::cout << "[DEBUG] Closing socket (fd: " << socket_fd << ")" << std::endl;
            close(socket_fd);
            socket_fd = -1;
        }
    }
};

#endif // TCPSOCKET_HPP
