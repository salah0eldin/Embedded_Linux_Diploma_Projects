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
#include <QMainWindow>

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
     * @param parent Parent widget (default: nullptr)
     */
    sensors_actuators(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~sensors_actuators();

// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    Ui::sensors_actuators *ui;
};

// ===================================================================
// END OF FILE
// ===================================================================
