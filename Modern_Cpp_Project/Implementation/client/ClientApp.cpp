// ===================================================================
// FILE: ClientApp.cpp
// DESCRIPTION: Implementation of ClientApp class
// AUTHOR: Salah-Eldin Hassen
// DATE: November 20, 2025
// ===================================================================

#include "ClientApp.hpp"

// ===================================================================
// CONSTRUCTORS & DESTRUCTOR
// ===================================================================

ClientApp::ClientApp(const ClientConfig& config)
    : config_(config),
      running_(false),
      current_temperature_(0.0f),
      temperature_threshold_(config.initial_threshold),
      led_state_(false) {
    
    PRINT_INFO("[ClientApp] Initialized");
}

ClientApp::~ClientApp() {
    stop();
}

// ===================================================================
// PUBLIC METHODS
// ===================================================================

bool ClientApp::start() {
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

void ClientApp::stop() {
    if (!running_) {
        return;
    }
    
    PRINT_INFO("[ClientApp] Stopping client application...");
    running_ = false;
    
    // Close sockets first to unblock any blocking operations
    closeSockets();
    
    // Wait for threads to finish
    if (threshold_monitor_thread_.joinable()) {
        threshold_monitor_thread_.join();
    }
    if (temperature_monitor_thread_.joinable()) {
        temperature_monitor_thread_.join();
    }
    
    PRINT_INFO("[ClientApp] Application stopped");
}

bool ClientApp::isRunning() const {
    return running_;
}

// ===================================================================
// PRIVATE METHODS - SOCKET MANAGEMENT
// ===================================================================

bool ClientApp::createSockets() {
    try {
        // Create TCP sockets
        host_tcp_socket_ = std::make_unique<TCPSocket>(false);
        sensor_tcp_socket_ = std::make_unique<TCPSocket>(false);
        
        // Create UDP socket for sending to host
        host_udp_socket_ = std::make_unique<UDPSocket>(
            config_.host_server.udp_ip,
            config_.host_server.udp_port
        );
        
        PRINT_DEBUG("[ClientApp] All sockets created successfully");
        return true;
        
    } catch (const std::exception& e) {
        PRINT_ERROR("[ClientApp] Failed to create sockets: " + std::string(e.what()));
        return false;
    }
}

void ClientApp::closeSockets() {
    if (host_tcp_socket_) host_tcp_socket_->shutdown();
    if (sensor_tcp_socket_) sensor_tcp_socket_->shutdown();
    if (host_udp_socket_) host_udp_socket_->shutdown();
}

// ===================================================================
// PRIVATE METHODS - THREAD FUNCTIONS
// ===================================================================

void ClientApp::sendLedState(bool led_on) {
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

void ClientApp::checkAndUpdateLedState() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    bool should_turn_on_led = (current_temperature_ > temperature_threshold_);
    
    // Update LED state if needed
    if (led_state_ != should_turn_on_led) {
        led_state_ = should_turn_on_led;
        
        PRINT_INFO("[LED Check] Temperature " + std::to_string(current_temperature_) + 
                  "°C " + (should_turn_on_led ? "exceeds" : "below") + 
                  " threshold " + std::to_string(temperature_threshold_) + 
                  "°C - LED turned " + (should_turn_on_led ? "ON" : "OFF"));
        
        // Send LED state update (unlock happens after this function returns)
        // We need to send outside the lock to avoid deadlock
        // So we'll use a flag approach or call sendLedState here
        sendLedState(should_turn_on_led);
    }
}

void ClientApp::onThresholdUpdate(const std::string& message) {
    // Parse threshold update (format: "THRESHOLD:25.5")
    if (message.find("THRESHOLD:") == 0) {
        try {
            float new_threshold = std::stof(message.substr(10));
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                temperature_threshold_ = new_threshold;
            }
            PRINT_INFO("[Threshold Monitor] Updated threshold to " + std::to_string(new_threshold) + "°C");
            
            // Check if LED state needs to be updated with new threshold
            checkAndUpdateLedState();
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[Threshold Monitor] Failed to parse threshold: " + std::string(e.what()));
        }
    }
}

void ClientApp::onTemperatureReceived(const std::string& message) {
    // Parse temperature (format: "TEMP:25.5")
    if (message.find("TEMP:") == 0) {
        try {
            float new_temp = std::stof(message.substr(5));
            
            // Update current temperature
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                current_temperature_ = new_temp;
            }
            
            PRINT_DEBUG("[Temperature Monitor] Received temperature: " + std::to_string(new_temp) + "°C");
            
            // Check if LED state needs to be updated with new temperature
            checkAndUpdateLedState();
            
            // Forward temperature to host
            if (host_udp_socket_->send(message) > 0) {
                PRINT_TRACE("[Temperature Monitor] Forwarded temperature to host");
            }
            
        } catch (const std::exception& e) {
            PRINT_ERROR("[Temperature Monitor] Failed to parse temperature: " + std::string(e.what()));
        }
    }
}

int ClientApp::receiveWithCallback(Socket* socket, char* buffer, int buffer_size, MessageCallback callback) {
    int bytes = socket->receive(buffer, buffer_size - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::string message(buffer);
        callback(message);
        return 1;  // Data received and processed
    } else if (bytes == 0) {
        return 0;  // No data available (would_block)
    } else {
        return -1;  // Connection closed or error
    }
}

void ClientApp::thresholdMonitorThreadFunc() {
    PRINT_INFO("[Threshold Monitor] Started");
    
    // Connect to host TCP server
    if (!host_tcp_socket_->connect(config_.host_server.tcp_ip, config_.host_server.tcp_port)) {
        PRINT_ERROR("[Threshold Monitor] Failed to connect to host server");
        running_ = false;
        return;  // Exit thread gracefully
    }
    PRINT_INFO("[Threshold Monitor] Connected to host server at " + 
               config_.host_server.tcp_ip + ":" + 
               std::to_string(config_.host_server.tcp_port));
    
    // Set socket to non-blocking mode
    host_tcp_socket_->setNonBlocking(true);
    
    char buffer[1024];
    
    while (running_) {
        int status = receiveWithCallback(
            host_tcp_socket_.get(),
            buffer,
            sizeof(buffer),
            [this](const std::string& msg) { onThresholdUpdate(msg); }
        );
        
        if (status > 0) {
            // Data received and processed
            continue;
        } else if (status == 0) {
            // No data available, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.check_interval));
        } else {
            // Connection closed or error
            PRINT_ERROR("[Threshold Monitor] Connection closed, stopping application");
            running_ = false;
            break;  // Exit loop and let thread terminate gracefully
        }
    }
    
    PRINT_INFO("[Threshold Monitor] Stopped");
}

void ClientApp::temperatureMonitorThreadFunc() {
    PRINT_INFO("[Temperature Monitor] Started");
    
    // Connect to sensor TCP server
    if (!sensor_tcp_socket_->connect(config_.sensor_actuator_server.tcp_ip, 
                                      config_.sensor_actuator_server.tcp_port)) {
        PRINT_ERROR("[Temperature Monitor] Failed to connect to sensor/actuator server");
        running_ = false;
        return;
    }
    PRINT_INFO("[Temperature Monitor] Connected to sensor/actuator server at " + 
               config_.sensor_actuator_server.tcp_ip + ":" + 
               std::to_string(config_.sensor_actuator_server.tcp_port));
    
    // Set TCP socket to non-blocking mode
    sensor_tcp_socket_->setNonBlocking(true);
    
    // Set remote endpoint for host UDP
    if (!host_udp_socket_->connect(config_.host_server.udp_ip, config_.host_server.udp_port)) {
        PRINT_ERROR("[Temperature Monitor] Failed to set host UDP endpoint");
        running_ = false;
        return;
    }
    
    char buffer[1024];
    
    while (running_) {
        // Request temperature from sensor via TCP
        std::string request = "GET_TEMP";
        int sent = sensor_tcp_socket_->send(request);
        if (sent > 0) {
            PRINT_TRACE("[Temperature Monitor] Sent temperature request");
            
            // Wait a bit for response
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Receive temperature response
            int bytes = sensor_tcp_socket_->receive(buffer, sizeof(buffer) - 1);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                std::string response(buffer);
                PRINT_TRACE("[Temperature Monitor] Received: " << response);
                
                // Process temperature data
                onTemperatureReceived(response);
            }
        }
        
        // Sleep to control request rate
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.check_interval));
    }
    
    PRINT_INFO("[Temperature Monitor] Stopped");
}

// ===================================================================
// END OF FILE
// ===================================================================
