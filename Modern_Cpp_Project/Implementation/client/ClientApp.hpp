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

// ===================================================================
// CLIENT APPLICATION CLASS
// ===================================================================
class ClientApp {
private:
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
    std::thread tcp_thread_;
    std::thread udp_thread_;
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
        tcp_thread_ = std::thread(&ClientApp::tcpThreadFunc, this);
        udp_thread_ = std::thread(&ClientApp::udpThreadFunc, this);
        
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
        if (tcp_thread_.joinable()) {
            tcp_thread_.join();
        }
        if (udp_thread_.joinable()) {
            udp_thread_.join();
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
                "0.0.0.0",  // Listen on all interfaces
                config_.sensor_actuator_server.udp_port,
                true  // Bind immediately
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
     * @brief TCP thread function
     * Handles:
     * - Connect to both servers via TCP
     * - Receive threshold updates from host
     * - Send LED state to both servers
     */
    void tcpThreadFunc() {
        PRINT_INFO("[TCP Thread] Started");
        
        // Connect to host TCP server
        if (!host_tcp_socket_->connect(config_.host_server.tcp_ip, config_.host_server.tcp_port)) {
            PRINT_ERROR("[TCP Thread] Failed to connect to host server");
            running_ = false;
            return;
        }
        PRINT_INFO("[TCP Thread] Connected to host server at " + 
                   config_.host_server.tcp_ip + ":" + 
                   std::to_string(config_.host_server.tcp_port));
        
        // Connect to sensor TCP server
        if (!sensor_tcp_socket_->connect(config_.sensor_actuator_server.tcp_ip, 
                                          config_.sensor_actuator_server.tcp_port)) {
            PRINT_ERROR("[TCP Thread] Failed to connect to sensor/actuator server");
            running_ = false;
            return;
        }
        PRINT_INFO("[TCP Thread] Connected to sensor/actuator server at " + 
                   config_.sensor_actuator_server.tcp_ip + ":" + 
                   std::to_string(config_.sensor_actuator_server.tcp_port));
        
        char buffer[1024];
        bool last_led_state = led_state_;
        
        while (running_) {
            // Check for threshold updates from host
            int bytes = host_tcp_socket_->receive(buffer, sizeof(buffer) - 1);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                std::string message(buffer);
                
                // Parse threshold update (format: "THRESHOLD:25.5")
                if (message.find("THRESHOLD:") == 0) {
                    try {
                        float new_threshold = std::stof(message.substr(10));
                        {
                            std::lock_guard<std::mutex> lock(state_mutex_);
                            temperature_threshold_ = new_threshold;
                        }
                        PRINT_INFO("[TCP Thread] Updated threshold to " + std::to_string(new_threshold) + "°C");
                    } catch (const std::exception& e) {
                        PRINT_ERROR("[TCP Thread] Failed to parse threshold: " + std::string(e.what()));
                    }
                }
            }
            
            // Send LED state if changed
            bool current_led_state;
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                current_led_state = led_state_;
            }
            
            if (current_led_state != last_led_state) {
                std::string led_message = "LED:" + std::string(current_led_state ? "ON" : "OFF");
                
                // Send to host
                if (host_tcp_socket_->send(led_message) > 0) {
                    PRINT_INFO("[TCP Thread] Sent LED state to host: " + 
                               std::string(current_led_state ? "ON" : "OFF"));
                }
                
                // Send to sensor/actuator
                if (sensor_tcp_socket_->send(led_message) > 0) {
                    PRINT_INFO("[TCP Thread] Sent LED state to sensor/actuator: " + 
                               std::string(current_led_state ? "ON" : "OFF"));
                }
                
                last_led_state = current_led_state;
            }
            
            // Sleep to avoid busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.check_interval));
        }
        
        PRINT_INFO("[TCP Thread] Stopped");
    }
    
    /**
     * @brief UDP thread function
     * Handles:
     * - Receive temperature from sensor/actuator via UDP
     * - Forward temperature to host via UDP
     * - Check threshold and update LED state
     */
    void udpThreadFunc() {
        PRINT_INFO("[UDP Thread] Started");
        
        // Set remote endpoint for host UDP
        if (!host_udp_socket_->connect(config_.host_server.udp_ip, config_.host_server.udp_port)) {
            PRINT_ERROR("[UDP Thread] Failed to set host UDP endpoint");
            running_ = false;
            return;
        }
        
        PRINT_INFO("[UDP Thread] Ready to receive temperature data on port " + 
                   std::to_string(config_.sensor_actuator_server.udp_port));
        
        char buffer[1024];
        
        while (running_) {
            // Receive temperature from sensor/actuator
            int bytes = sensor_udp_socket_->receive(buffer, sizeof(buffer) - 1);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                std::string message(buffer);
                
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
                                PRINT_INFO("[UDP Thread] Temperature " + std::to_string(new_temp) + 
                                          "°C " + (should_turn_on_led ? "exceeds" : "below") + 
                                          " threshold " + std::to_string(threshold) + 
                                          "°C - LED turned " + (should_turn_on_led ? "ON" : "OFF"));
                            }
                        }
                        
                        PRINT_DEBUG("[UDP Thread] Received temperature: " + std::to_string(new_temp) + "°C");
                        
                        // Forward temperature to host
                        if (host_udp_socket_->send(message) > 0) {
                            PRINT_TRACE("[UDP Thread] Forwarded temperature to host");
                        }
                        
                    } catch (const std::exception& e) {
                        PRINT_ERROR("[UDP Thread] Failed to parse temperature: " + std::string(e.what()));
                    }
                }
            }
            
            // Small sleep to avoid busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        PRINT_INFO("[UDP Thread] Stopped");
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
