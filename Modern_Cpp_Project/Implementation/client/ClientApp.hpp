// ===================================================================
// FILE: ClientApp.hpp
// DESCRIPTION: Main application class managing dual-server connections with threading
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include "TCPSocket.hpp"
#include "UDPSocket.hpp"
#include "ConfigParser.hpp"
#include "config.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <functional>

// ===================================================================
// CLIENT APPLICATION CLASS
// ===================================================================
class ClientApp {
private:
    // ===================================================================
    // TYPE DEFINITIONS
    // ===================================================================
    using MessageCallback = std::function<void(const std::string&)>;
    
    // ===================================================================
    // MEMBER VARIABLES
    // ===================================================================
    
    // Configuration
    ClientConfig config_;
    
    // TCP Sockets
    std::unique_ptr<TCPSocket> host_tcp_socket_;
    std::unique_ptr<TCPSocket> sensor_tcp_socket_;
    
    // UDP Socket (only for host server)
    std::unique_ptr<UDPSocket> host_udp_socket_;
    
    // Threading
    std::thread threshold_monitor_thread_;
    std::thread temperature_monitor_thread_;
    std::atomic<bool> running_;
    
    // Shared State (protected by mutex)
    mutable std::mutex state_mutex_;  // mutable allows locking in const methods
    float current_temperature_;
    float temperature_threshold_;
    bool led_state_;
    
public:
    // ===================================================================
    // CONSTRUCTORS & DESTRUCTOR
    // ===================================================================
    
    /**
     * @brief Constructor
     * @param config Client configuration
     */
    explicit ClientApp(const ClientConfig& config);
    
    /**
     * @brief Destructor
     */
    ~ClientApp();
    
    // ===================================================================
    // PUBLIC METHODS
    // ===================================================================
    
    /**
     * @brief Start the client application
     * @return true if started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stop the client application
     */
    void stop();
    
    /**
     * @brief Check if application is running
     * @return true if running, false otherwise
     */
    bool isRunning() const;
    
private:
    // ===================================================================
    // PRIVATE METHODS - SOCKET MANAGEMENT
    // ===================================================================
    
    /**
     * @brief Create all sockets
     * @return true if successful, false otherwise
     */
    bool createSockets();
    
    /**
     * @brief Close all sockets
     */
    void closeSockets();
    
    // ===================================================================
    // PRIVATE METHODS - THREAD FUNCTIONS
    // ===================================================================
    
    /**
     * @brief Send LED state to both TCP sockets
     * @param led_on LED state (true=ON, false=OFF)
     */
    void sendLedState(bool led_on);
    
    /**
     * @brief Check temperature against threshold and update LED state if needed
     * Must be called with state_mutex_ locked OR will lock internally
     */
    void checkAndUpdateLedState();
    
    /**
     * @brief Callback for handling threshold updates from host
     * @param message The received message
     */
    void onThresholdUpdate(const std::string& message);
    
    /**
     * @brief Callback for handling temperature data from sensor
     * @param message The received message
     */
    void onTemperatureReceived(const std::string& message);
    
    /**
     * @brief Generic message receiver with callback
     * @param socket Pointer to socket to receive from
     * @param buffer Buffer for received data
     * @param buffer_size Size of buffer
     * @param callback Callback function to handle received message
     * @return 1 if data received and processed, 0 if no data available, -1 if connection closed/error
     */
    int receiveWithCallback(Socket* socket, char* buffer, int buffer_size, MessageCallback callback);
    
    /**
     * @brief Threshold monitor thread function
     * Handles:
     * - Connect to host server via TCP
     * - Receive threshold updates from host server (blocking - waits for data)
     */
    void thresholdMonitorThreadFunc();
    
    /**
     * @brief Temperature monitor thread function
     * Handles:
     * - Connect to sensor/actuator server via TCP (for LED state)
     * - Communicate with sensor/actuator via UDP (send initial request, then receive temperature)
     * - Forward temperature to host via UDP
     * - Check threshold and update LED state
     * - Send LED state updates to both servers via TCP
     */
    void temperatureMonitorThreadFunc();
};

// ===================================================================
// END OF FILE
// ===================================================================
