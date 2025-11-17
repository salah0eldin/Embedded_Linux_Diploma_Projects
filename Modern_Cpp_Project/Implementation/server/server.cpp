#include "TCPSocket.hpp"
#include "ServerChannel.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "[DEBUG] Starting TCP Server Application..." << std::endl;
    
    try {
        // Create TCP Socket (server mode)
        TCPSocket* tcpSocket = new TCPSocket(8080, "127.0.0.1", true);
        
        // Create Server Channel with the socket
        ServerChannel serverChannel(tcpSocket);
        
        // Start the server channel (wait for client connection)
        serverChannel.start();
        
        // Send a message to client
        serverChannel.send("Hello from Server!");
        
        // Receive message from client
        serverChannel.receive();
        
        // Keep connection alive for a moment
        std::cout << "[DEBUG] Keeping connection open for 5 seconds..." << std::endl;
        sleep(5);
        
        // Stop the server channel
        serverChannel.stop();
        
        // Cleanup
        delete tcpSocket;
        
        std::cout << "[DEBUG] Server application shutdown complete" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
