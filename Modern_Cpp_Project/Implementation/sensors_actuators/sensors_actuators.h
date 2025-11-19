// ===================================================================
// FILE: sensors_actuators.h
// DESCRIPTION: Sensors and Actuators main window class declaration
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
// Include RapidYAML before Qt headers to avoid 'emit' macro conflict
// See: https://github.com/biojppm/rapidyaml/issues/120
#include "SensorsActuatorsConfigParser.hpp"
#include "TCPSocket.hpp"

#include <QMainWindow>
#include <QString>
#include <memory>
#include <thread>
#include <atomic>

// ===================================================================
// FORWARD DECLARATIONS
// ===================================================================
QT_BEGIN_NAMESPACE
namespace Ui {
class sensors_actuators;
}
QT_END_NAMESPACE

// ===================================================================
// SENSORS_ACTUATORS CLASS
// ===================================================================
/**
 * @brief Main window for sensors and actuators application
 * Handles GUI and sensor/actuator control functionality
 */
class sensors_actuators : public QMainWindow
{
    Q_OBJECT

// ===================================================================
// PUBLIC METHODS
// ===================================================================
public:
    /**
     * @brief Constructor
     * @param configFile Path to YAML configuration file
     * @param parent Parent widget (default: nullptr)
     */
    sensors_actuators(const QString& configFile, QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~sensors_actuators();

// ===================================================================
// PRIVATE SLOTS
// ===================================================================
private slots:
    /**
     * @brief Handle temperature slider value changes
     * @param value Slider value (temperature * 10)
     */
    void onTemperatureChanged(int value);
    
    /**
     * @brief Update LED status display
     * @param ledOn LED state (true = ON, false = OFF)
     */
    void updateLedStatus(bool ledOn);

// ===================================================================
// PRIVATE METHODS
// ===================================================================
private:
    /**
     * @brief Initialize network connections
     */
    void initializeNetwork();
    
    /**
     * @brief Start communication threads
     */
    void startCommunication();
    
    /**
     * @brief Stop communication threads
     */
    void stopCommunication();
    
    /**
     * @brief TCP listener thread function (handles client requests and LED commands)
     */
    void tcpListenerThread();

// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    Ui::sensors_actuators *ui;
    SensorsActuatorsConfig config_;
    
    // Network socket
    std::unique_ptr<TCPSocket> tcp_socket_;
    
    // Communication thread
    std::thread tcp_thread_;
    std::atomic<bool> running_;
    
    // Current state
    float current_temperature_;
};

// ===================================================================
// END OF FILE
// ===================================================================
