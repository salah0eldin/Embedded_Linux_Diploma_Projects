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
#include <chrono>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>
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
    
    // UDP Sockets
    std::unique_ptr<UDPSocket> host_udp_socket_;           // Send temp to host
    std::unique_ptr<UDPSocket> sensor_udp_socket_;         // Receive temp from sensor
    
    // Threading
    std::thread  threshold_monitor_thread_;
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
    explicit ClientApp(const ClientConfig& config)
        : config_(config),
          running_(false),
          current_temperature_(0.0f),
          temperature_threshold_(config.initial_threshold),
          led_state_(false) {
        
        PRINT_INFO("[ClientApp] Initialized with threshold: " + 
                   std::to_string(config_.initial_threshold) + "°C");
    }
    
    /**
     * @brief Destructor
     */
    ~ClientApp() {
        stop();
    }
    
    // ===================================================================
    // PUBLIC METHODS
    // ===================================================================
    
    /**
     * @brief Start the client application
     * @return true if started successfully, false otherwise
     */
    bool start() {
        if (running_) {
            PRINT_ERROR("[ClientApp] Already running");
            return false;
        }
        
        PRINT_INFO("[ClientApp] Starting client application...");
        
        // Create sockets
        if (!createSockets()) {
            PRINT_ERROR("[ClientApp] Failed to create sockets");
            return false;
        }
        
        // Start threads
        running_ = true;
        threshold_monitor_thread_ = std::thread(&ClientApp::thresholdMonitorThreadFunc, this);
        temperature_monitor_thread_ = std::thread(&ClientApp::temperatureMonitorThreadFunc, this);
        
        PRINT_INFO("[ClientApp] Application started successfully");
        return true;
    }
    
    /**
     * @brief Stop the client application
     */
    void stop() {
        if (!running_) {
            return;
        }
        
        PRINT_INFO("[ClientApp] Stopping client application...");
        running_ = false;
        
        // Wait for threads to finish
        if (threshold_monitor_thread_.joinable()) {
            threshold_monitor_thread_.join();
        }
        if (temperature_monitor_thread_.joinable()) {
            temperature_monitor_thread_.join();
        }
        
        // Close sockets
        closeSockets();
        
        PRINT_INFO("[ClientApp] Application stopped");
    }
    
    /**
     * @brief Check if application is running
     * @return true if running, false otherwise
     */
    bool isRunning() const {
        return running_;
    }
    
    /**
     * @brief Get current temperature
     * @return Current temperature value
     */
    float getCurrentTemperature() const {
        std::lock_guard<std::mutex> lock(state_mutex_);
        return current_temperature_;
    }
    
    /**
     * @brief Get current threshold
     * @return Current threshold value
     */
    float getThreshold() const {
        std::lock_guard<std::mutex> lock(state_mutex_);
        return temperature_threshold_;
    }
    
    /**
     * @brief Get LED state
     * @return Current LED state (true=ON, false=OFF)
     */
    bool getLedState() const {
        std::lock_guard<std::mutex> lock(state_mutex_);
        return led_state_;
    }
    
private:
    // ===================================================================
    // PRIVATE METHODS - SOCKET MANAGEMENT
    // ===================================================================
    
    /**
     * @brief Create all sockets
     * @return true if successful, false otherwise
     */
    bool createSockets() {
        try {
            // Create TCP sockets
            host_tcp_socket_ = std::make_unique<TCPSocket>(false);
            sensor_tcp_socket_ = std::make_unique<TCPSocket>(false);
            
            // Create UDP sockets
            // For sending to host
            host_udp_socket_ = std::make_unique<UDPSocket>(
                config_.host_server.udp_ip,
                config_.host_server.udp_port
            );
            
            // For receiving from sensor (bind immediately)
            sensor_udp_socket_ = std::make_unique<UDPSocket>(
                config_.sensor_actuator_server.udp_ip,  
                config_.sensor_actuator_server.udp_port
            );
            
            PRINT_DEBUG("[ClientApp] All sockets created successfully");
            return true;
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[ClientApp] Failed to create sockets: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Close all sockets
     */
    void closeSockets() {
        if (host_tcp_socket_) host_tcp_socket_->shutdown();
        if (sensor_tcp_socket_) sensor_tcp_socket_->shutdown();
        if (host_udp_socket_) host_udp_socket_->shutdown();
        if (sensor_udp_socket_) sensor_udp_socket_->shutdown();
    }
    
    // ===================================================================
    // PRIVATE METHODS - THREAD FUNCTIONS
    // ===================================================================
    
    /**
     * @brief Send LED state to both TCP sockets
     * @param led_on LED state (true=ON, false=OFF)
     */
    void sendLedState(bool led_on) {
        std::string led_message = "LED:" + std::string(led_on ? "ON" : "OFF");
        
        // Send to host
        if (host_tcp_socket_ && host_tcp_socket_->send(led_message) > 0) {
            PRINT_INFO("[LED Update] Sent to host: " + std::string(led_on ? "ON" : "OFF"));
        }
        
        // Send to sensor/actuator
        if (sensor_tcp_socket_ && sensor_tcp_socket_->send(led_message) > 0) {
            PRINT_INFO("[LED Update] Sent to sensor/actuator: " + std::string(led_on ? "ON" : "OFF"));
        }
    }
    
    /**
     * @brief Callback for handling threshold updates from host
     * @param message The received message
     */
    void onThresholdUpdate(const std::string& message) {
        // Parse threshold update (format: "THRESHOLD:25.5")
        if (message.find("THRESHOLD:") == 0) {
            try {
                float new_threshold = std::stof(message.substr(10));
                {
                    std::lock_guard<std::mutex> lock(state_mutex_);
                    temperature_threshold_ = new_threshold;
                }
                PRINT_INFO("[Threshold Monitor] Updated threshold to " + std::to_string(new_threshold) + "°C");
            } catch (const std::exception& e) {
                PRINT_ERROR("[Threshold Monitor] Failed to parse threshold: " + std::string(e.what()));
            }
        }
    }
    
    /**
     * @brief Callback for handling temperature data from sensor
     * @param message The received message
     */
    void onTemperatureReceived(const std::string& message) {
        // Parse temperature (format: "TEMP:25.5")
        if (message.find("TEMP:") == 0) {
            try {
                float new_temp = std::stof(message.substr(5));
                
                // Update current temperature
                float threshold;
                bool should_turn_on_led;
                {
                    std::lock_guard<std::mutex> lock(state_mutex_);
                    current_temperature_ = new_temp;
                    threshold = temperature_threshold_;
                    should_turn_on_led = (new_temp > threshold);
                    
                    // Update LED state if needed
                    if (led_state_ != should_turn_on_led) {
                        led_state_ = should_turn_on_led;
                        PRINT_INFO("[Temperature Monitor] Temperature " + std::to_string(new_temp) + 
                                  "°C " + (should_turn_on_led ? "exceeds" : "below") + 
                                  " threshold " + std::to_string(threshold) + 
                                  "°C - LED turned " + (should_turn_on_led ? "ON" : "OFF"));
                    }
                }
                
                // Send LED state immediately if changed
                if (should_turn_on_led != led_state_) {
                    sendLedState(should_turn_on_led);
                }
                
                PRINT_DEBUG("[Temperature Monitor] Received temperature: " + std::to_string(new_temp) + "°C");
                
                // Forward temperature to host
                if (host_udp_socket_->send(message) > 0) {
                    PRINT_TRACE("[Temperature Monitor] Forwarded temperature to host");
                }
                
            } catch (const std::exception& e) {
                PRINT_ERROR("[Temperature Monitor] Failed to parse temperature: " + std::string(e.what()));
            }
        }
    }
    
    /**
     * @brief Generic message receiver with callback
     * @param socket Pointer to socket to receive from
     * @param buffer Buffer for received data
     * @param buffer_size Size of buffer
     * @param callback Callback function to handle received message
     */
    void receiveWithCallback(Socket* socket, char* buffer, int buffer_size, MessageCallback callback) {
        int bytes = socket->receive(buffer, buffer_size - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string message(buffer);
            callback(message);
        }
    }
    
    /**
     * @brief Threshold monitor thread function
     * Handles:
     * - Connect to host server via TCP
     * - Receive threshold updates from host server (blocking - waits for data)
     */
    void thresholdMonitorThreadFunc() {
        PRINT_INFO("[Threshold Monitor] Started");
        
        // Connect to host TCP server
        if (!host_tcp_socket_->connect(config_.host_server.tcp_ip, config_.host_server.tcp_port)) {
            PRINT_ERROR("[Threshold Monitor] Failed to connect to host server");
            running_ = false;
            return;
        }
        PRINT_INFO("[Threshold Monitor] Connected to host server at " + 
                   config_.host_server.tcp_ip + ":" + 
                   std::to_string(config_.host_server.tcp_port));
        
        // Keep socket in blocking mode - will wait for data
        char buffer[1024];
        
        while (running_) {
            // Blocking receive - callback triggered when data arrives
            receiveWithCallback(
                host_tcp_socket_.get(), 
                buffer, 
                sizeof(buffer),
                [this](const std::string& msg) { onThresholdUpdate(msg); }
            );
        }
        
        PRINT_INFO("[Threshold Monitor] Stopped");
    }
    
    /**
     * @brief Temperature monitor thread function
     * Handles:
     * - Connect to sensor/actuator server via TCP (for LED state)
     * - Communicate with sensor/actuator via UDP (send initial request, then receive temperature)
     * - Forward temperature to host via UDP
     * - Check threshold and update LED state
     * - Send LED state updates to both servers via TCP
     */
    void temperatureMonitorThreadFunc() {
        PRINT_INFO("[Temperature Monitor] Started");
        
        // Connect to sensor TCP server for LED control
        if (!sensor_tcp_socket_->connect(config_.sensor_actuator_server.tcp_ip, 
                                          config_.sensor_actuator_server.tcp_port)) {
            PRINT_ERROR("[Temperature Monitor] Failed to connect to sensor/actuator server");
            running_ = false;
            return;
        }
        PRINT_INFO("[Temperature Monitor] Connected to sensor/actuator server at " + 
                   config_.sensor_actuator_server.tcp_ip + ":" + 
                   std::to_string(config_.sensor_actuator_server.tcp_port));
        
        // Set remote endpoint for sensor UDP
        if (!sensor_udp_socket_->connect(config_.sensor_actuator_server.udp_ip, 
                                          config_.sensor_actuator_server.udp_port)) {
            PRINT_ERROR("[Temperature Monitor] Failed to connect to sensor/actuator UDP");
            running_ = false;
            return;
        }
        
        // Set remote endpoint for host UDP
        if (!host_udp_socket_->connect(config_.host_server.udp_ip, config_.host_server.udp_port)) {
            PRINT_ERROR("[Temperature Monitor] Failed to set host UDP endpoint");
            running_ = false;
            return;
        }
        
        PRINT_INFO("[Temperature Monitor] Ready to communicate with sensor/actuator at " + 
                   config_.sensor_actuator_server.udp_ip + ":" + 
                   std::to_string(config_.sensor_actuator_server.udp_port));
        
        // Keep UDP socket in blocking mode - will wait for data
        char buffer[1024];
        
        while (running_) {
            // Blocking receive - callback triggered when data arrives
            receiveWithCallback(
                sensor_udp_socket_.get(),
                buffer,
                sizeof(buffer),
                [this](const std::string& msg) { onTemperatureReceived(msg); }
            );
            
            // Sleep to control receive rate
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.check_interval));
        }
        
        PRINT_INFO("[Temperature Monitor] Stopped");
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
