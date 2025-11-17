#include "TCPSocket.hpp"
#include "ClientChannel.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "[DEBUG] Starting TCP Client Application..." << std::endl;
    
    try {
        // Create TCP Socket (client mode)
        TCPSocket* tcpSocket = new TCPSocket(8080, "127.0.0.1", false);
        
        // Create Client Channel with the socket
        ClientChannel clientChannel(tcpSocket);
        
        // Start the client channel (connect to server)
        clientChannel.start();
        
        // Receive message from server
        clientChannel.receive();
        
        // Send a message to server
        clientChannel.send("Hello from Client!");
        
        // Keep connection alive for a moment
        std::cout << "[DEBUG] Keeping connection open for 5 seconds..." << std::endl;
        sleep(5);
        
        // Stop the client channel
        clientChannel.stop();
        
        // Cleanup
        delete tcpSocket;
        
        std::cout << "[DEBUG] Client application shutdown complete" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
