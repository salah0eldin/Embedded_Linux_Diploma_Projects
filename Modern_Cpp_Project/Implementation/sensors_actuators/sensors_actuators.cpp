// ===================================================================
// FILE: sensors_actuators.cpp
// DESCRIPTION: Sensors and Actuators main window class implementation
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
// Include config parser (with RapidYAML) before Qt headers
#include "SensorsActuatorsConfigParser.hpp"
#include "config.h"

#include "sensors_actuators.h"
#include "./ui_sensors_actuators.h"

// ===================================================================
// CONSTRUCTOR
// ===================================================================
sensors_actuators::sensors_actuators(const QString& configFile, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::sensors_actuators)
    , running_(false)
    , current_temperature_(25.0f)
{
    ui->setupUi(this);
    
    // Load configuration from YAML file
    config_ = SensorsActuatorsConfigParser::parseConfig(configFile.toStdString());
    
    PRINT_INFO("Loaded configuration from: " << configFile.toStdString());
    PRINT_INFO("TCP Server: " << config_.tcp_ip << ":" << config_.tcp_port);
    
    // Initialize slider with default values (0.0 to 100.0)
    // Multiply by 10 to allow 0.1°C precision with integer slider
    ui->sliderTemperature->setMinimum(0);     // 0.0°C
    ui->sliderTemperature->setMaximum(1000);  // 100.0°C
    ui->sliderTemperature->setValue(275);     // 27.5°C
    
    // Update min/max labels
    ui->labelMinTemp->setText("0.0°C");
    ui->labelMaxTemp->setText("100.0°C");
    
    // Connect slider to temperature display update
    connect(ui->sliderTemperature, &QSlider::valueChanged, this, &sensors_actuators::onTemperatureChanged);
    
    // Initialize temperature display
    onTemperatureChanged(ui->sliderTemperature->value());
    
    // Initialize network and start communication
    initializeNetwork();
    startCommunication();
    
    PRINT_INFO("Sensors & Actuators window initialized");
}

// ===================================================================
// DESTRUCTOR
// ===================================================================
sensors_actuators::~sensors_actuators()
{
    PRINT_INFO("Sensors & Actuators window closing");
    
    // Stop communication
    stopCommunication();
    
    delete ui;
}

// ===================================================================
// SLOTS
// ===================================================================
void sensors_actuators::onTemperatureChanged(int value)
{
    // Convert slider value back to temperature (divide by 10)
    float temperature = value / 10.0f;
    current_temperature_ = temperature;
    
    // Update temperature display
    ui->labelTemperatureValue->setText(QString::number(temperature, 'f', 1) + "°C");
    
    PRINT_DEBUG("Temperature changed: " << temperature << "°C");
}

void sensors_actuators::updateLedStatus(bool ledOn)
{
    // Update LED status display based on command from client
    QString ledText = ledOn ? "💡 LED Status: ON" : "💡 LED Status: OFF";
    QString ledColor = ledOn ? "color: #ffff00;" : "color: #e0e0e0;";
    
    ui->labelLedStatus->setText(ledText);
    ui->labelLedStatus->setStyleSheet(
        "QLabel {"
        "    background-color: rgba(0, 0, 0, 0.3);"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "    " + ledColor +
        "}"
    );
    
    PRINT_INFO("LED status updated: " << (ledOn ? "ON" : "OFF"));
}

// ===================================================================
// PRIVATE METHODS
// ===================================================================
void sensors_actuators::initializeNetwork()
{
    PRINT_INFO("Initializing network connections...");
    
    // Create TCP socket as server (listening for client connections)
    tcp_socket_ = std::make_unique<TCPSocket>(
        config_.tcp_port, 
        config_.tcp_ip, 
        true  // server mode - listen for incoming connections
    );
    
    PRINT_INFO("TCP socket initialized");
    PRINT_INFO("TCP listening on: " << config_.tcp_ip << ":" << config_.tcp_port);
}

void sensors_actuators::startCommunication()
{
    PRINT_INFO("Starting communication thread...");
    
    running_ = true;
    tcp_thread_ = std::thread(&sensors_actuators::tcpListenerThread, this);
    
    PRINT_INFO("Communication started");
}

void sensors_actuators::stopCommunication()
{
    if (!running_) {
        return;
    }
    
    PRINT_INFO("Stopping communication...");
    running_ = false;
    
    // Shutdown sockets to unblock threads
    if (tcp_socket_) {
        tcp_socket_->shutdown();
    }
    
    // Wait for thread to finish: try to join with a short timeout, otherwise detach
    auto try_join_with_timeout = [](std::thread& thr, const char* name, int timeout_ms = 500) {
        if (!thr.joinable()) return;
        using clk = std::chrono::steady_clock;
        auto start = clk::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(clk::now() - start).count() < timeout_ms) {
            // Sleep briefly and check if thread is still joinable
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (!thr.joinable()) return;
        }
        // If still running, detach to allow process to exit; log a warning
        PRINT_ERROR(std::string("Thread '") + name + "' did not stop in time - detaching");
        try {
            thr.detach();
        } catch (...) {
            // ignore
        }
    };

    try_join_with_timeout(tcp_thread_, "TCP Thread");
    
    PRINT_INFO("Communication stopped");
}

void sensors_actuators::tcpListenerThread()
{
    PRINT_INFO("[TCP Thread] Started - Waiting for client connection...");
    
    // Wait for client to connect (server mode)
    if (!tcp_socket_->waitForConnect()) {
        PRINT_ERROR("[TCP Thread] Failed to accept client connection");
        return;
    }
    
    PRINT_INFO("[TCP Thread] Client connected");
    
    // Set socket to non-blocking mode
    tcp_socket_->setNonBlocking(true);
    
    char buffer[1024];
    while (running_) {
        int bytes = tcp_socket_->receive(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string message(buffer);
            PRINT_DEBUG("[TCP Thread] Received: " << message);
            
            // Handle temperature request (format: "GET_TEMP")
            if (message.find("GET_TEMP") == 0) {
                // Send temperature response (format: "TEMP:25.5")
                std::string response = "TEMP:" + std::to_string(current_temperature_);
                int sent = tcp_socket_->send(response);
                if (sent > 0) {
                    PRINT_DEBUG("[TCP Thread] Sent temperature: " << current_temperature_ << "°C");
                } else {
                    PRINT_ERROR("[TCP Thread] Failed to send temperature");
                }
            }
            // Handle LED commands (format: "LED:ON" or "LED:OFF")
            else if (message.find("LED:") == 0) {
                bool ledOn = (message.find("LED:ON") == 0);
                PRINT_INFO("[TCP Thread] Received LED command: " << (ledOn ? "ON" : "OFF"));
                
                // Update LED status from main thread
                QMetaObject::invokeMethod(this, [this, ledOn]() {
                    updateLedStatus(ledOn);
                }, Qt::QueuedConnection);
            }
        } else if (bytes < 0) {
            // Error occurred (could be connection closed)
            PRINT_ERROR("[TCP Thread] Error receiving data, client may have disconnected");
            break;
        } else {
            // bytes == 0 in non-blocking mode means no data available, not disconnected
            // No data available, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    PRINT_INFO("[TCP Thread] Stopped");
}

// ===================================================================
// END OF FILE
// ===================================================================
