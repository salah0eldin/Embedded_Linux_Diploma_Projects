// ===================================================================
// FILE: Client.cpp
// DESCRIPTION: TCP Client application main entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "TCPSocket.hpp"
#include "ClientChannel.hpp"
#include "ConfigParser.hpp"
#include "config.h"
#include <unistd.h>
#include <cstring>

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Define the global log level variable
int g_current_log_level = LOG_LEVEL_INFO; // Default to INFO level

// ===================================================================
// HELPER FUNCTIONS
// ===================================================================
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c, --config FILE        Configuration file path" << std::endl;
    std::cout << "                           Default: client_config.yaml" << std::endl;
    std::cout << "  -l, --log-level LEVEL    Set log level (none/error/info/debug/trace)" << std::endl;
    std::cout << "                           Overrides config file setting" << std::endl;
    std::cout << "  -h, --help               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " -c my_config.yaml" << std::endl;
    std::cout << "  " << programName << " -l debug" << std::endl;
    std::cout << "  " << programName << " --config client_config.yaml --log-level trace" << std::endl;
}

/**
 * @brief Parse command-line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param configFile Reference to config file path (output)
 * @param logLevelOverride Reference to log level override (output)
 * @return 0 on success, non-zero on error or help requested
 */
int parseCommandLineArgs(int argc, char* argv[], std::string& configFile, std::string& logLevelOverride) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) {
                configFile = argv[i + 1];
                i++; // Skip next argument
            } else {
                std::cerr << "Error: -c/--config requires an argument" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log-level") == 0) {
            if (i + 1 < argc) {
                logLevelOverride = argv[i + 1];
                i++; // Skip next argument
            } else {
                std::cerr << "Error: -l/--log-level requires an argument" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 1; // Return non-zero to indicate early exit
        } else {
            std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    return 0; // Success
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main(int argc, char* argv[]) {
    // ---------------------------------------------------------------
    // Parse command-line arguments
    // ---------------------------------------------------------------
    std::string configFile = "client_config.yaml";
    std::string logLevelOverride = "";
    
    if (parseCommandLineArgs(argc, argv, configFile, logLevelOverride) != 0) {
        return 0; // Exit if help was shown or error occurred
    }
    
    // ---------------------------------------------------------------
    // Load configuration from YAML file
    // ---------------------------------------------------------------
    PRINT_DEBUG("Loading configuration from: " << configFile);
    ClientConfig config = ConfigParser::parseConfig(configFile);
    
    // Apply log level from config file
    setLogLevel(config.log_level);
    
    // Override with command-line argument if provided
    if (!logLevelOverride.empty()) {
        setLogLevel(logLevelOverride);
        PRINT_DEBUG("Log level overridden by command line: " << logLevelOverride);
    }
    
    PRINT_INFO("Starting TCP Client Application...");
    PRINT_DEBUG("Server IP: " << config.server_ip);
    PRINT_DEBUG("Server Port: " << config.server_port);
    PRINT_DEBUG("Connection timeout: " << config.connection_timeout << " seconds");
    PRINT_DEBUG("Retry attempts: " << config.retry_attempts);
    
    // ---------------------------------------------------------------
    // Main application logic
    // ---------------------------------------------------------------
    try {
        // Create TCP Socket (client mode) using config values
        TCPSocket* tcpSocket = new TCPSocket(config.server_port, config.server_ip, false);
        
        // Create Client Channel with the socket
        ClientChannel clientChannel(tcpSocket);
        
        // Start the client channel (connect to server)
        clientChannel.start();
        
        // Receive message from server
        clientChannel.receive();
        
        // Send a message to server
        clientChannel.send("Hello from Client!");
        
        // Keep connection alive for a moment
        PRINT_DEBUG("Keeping connection open for " << config.connection_timeout << " seconds...");
        sleep(config.connection_timeout);
        
        // Stop the client channel
        clientChannel.stop();
        
        // Cleanup
        delete tcpSocket;
        
        PRINT_INFO("Client application shutdown complete");
        
    } catch (const std::exception& e) {
        PRINT_ERROR("Exception: " << e.what());
        return 1;
    }
    
    return 0;
}

// ===================================================================
// END OF FILE
// ===================================================================
