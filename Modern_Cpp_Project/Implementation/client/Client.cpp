// ===================================================================
// FILE: Client.cpp
// DESCRIPTION: TCP Client application main entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "ClientApp.hpp"
#include "ConfigParser.hpp"
#include "config.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Define the global log level variable
int g_current_log_level = LOG_LEVEL_INFO; // Default to INFO level

// Global pointer to ClientApp for signal handling
std::unique_ptr<ClientApp> g_app = nullptr;

// ===================================================================
// HELPER FUNCTIONS
// ===================================================================

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number
 */
void signalHandler(int signum) {
    PRINT_INFO("Interrupt signal (" << signum << ") received");
    if (g_app) {
        g_app->stop();
    }
}

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
    std::string configFile = "../config.yaml";
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
    
    PRINT_INFO("=======================================================");
    PRINT_INFO("       Multi-Server Client Application");
    PRINT_INFO("=======================================================");
    PRINT_DEBUG("Host Server TCP: " << config.host_server.tcp_ip << ":" << config.host_server.tcp_port);
    PRINT_DEBUG("Host Server UDP: " << config.host_server.udp_ip << ":" << config.host_server.udp_port);
    PRINT_DEBUG("Sensor Server TCP: " << config.sensor_actuator_server.tcp_ip << ":" << config.sensor_actuator_server.tcp_port);
    PRINT_DEBUG("Sensor Server UDP: " << config.sensor_actuator_server.udp_ip << ":" << config.sensor_actuator_server.udp_port);
    PRINT_DEBUG("Initial Threshold: " << config.initial_threshold << "°C");
    PRINT_DEBUG("Check Interval: " << config.check_interval << "ms");
    
    // ---------------------------------------------------------------
    // Setup signal handlers for graceful shutdown
    // ---------------------------------------------------------------
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // ---------------------------------------------------------------
    // Main application logic
    // ---------------------------------------------------------------
    try {
        // Create and start the client application
        g_app = std::make_unique<ClientApp>(config);
        
        if (!g_app->start()) {
            PRINT_ERROR("Failed to start client application");
            return 1;
        }
        
        PRINT_INFO("Application running. Press Ctrl+C to stop.");
        PRINT_INFO("=======================================================");
        
        // Keep running until stopped by signal
        while (g_app->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Optionally print status every few seconds
            static int counter = 0;
            if (++counter % 10 == 0) {
                PRINT_DEBUG("Status - Temp: " << g_app->getCurrentTemperature() << 
                           "°C, Threshold: " << g_app->getThreshold() << 
                           "°C, LED: " << (g_app->getLedState() ? "ON" : "OFF"));
            }
        }
        
        PRINT_INFO("Client application shutdown complete");
        
    } catch (const std::exception& e) {
        PRINT_ERROR("Exception: " << e.what());
        if (g_app) {
            g_app->stop();
        }
        return 1;
    }
    
    return 0;
}

// ===================================================================
// END OF FILE
// ===================================================================
