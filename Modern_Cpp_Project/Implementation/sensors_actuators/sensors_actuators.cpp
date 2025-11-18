// ===================================================================
// FILE: sensors_actuators.cpp
// DESCRIPTION: Sensors and Actuators main window class implementation
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "sensors_actuators.h"
#include "./ui_sensors_actuators.h"
#include "config.h"

// ===================================================================
// CONSTRUCTOR
// ===================================================================
sensors_actuators::sensors_actuators(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::sensors_actuators)
{
    ui->setupUi(this);
    
    PRINT_INFO("Sensors & Actuators window initialized");
}

// ===================================================================
// DESTRUCTOR
// ===================================================================
sensors_actuators::~sensors_actuators()
{
    PRINT_INFO("Sensors & Actuators window closing");
    delete ui;
}

// ===================================================================
// END OF FILE
// ===================================================================
