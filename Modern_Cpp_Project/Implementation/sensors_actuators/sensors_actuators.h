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

#include <QMainWindow>
#include <QString>

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

// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    Ui::sensors_actuators *ui;
    SensorsActuatorsConfig config_;
};

// ===================================================================
// END OF FILE
// ===================================================================
