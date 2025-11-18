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
{
    ui->setupUi(this);
    
    // Load configuration from YAML file
    config_ = SensorsActuatorsConfigParser::parseConfig(configFile.toStdString());
    
    PRINT_INFO("Loaded configuration from: " << configFile.toStdString());
    PRINT_INFO("Server TCP: " << config_.server_tcp_ip << ":" << config_.server_tcp_port);
    PRINT_INFO("Server UDP Port: " << config_.server_udp_port);
    PRINT_INFO("Temperature Range: " << config_.temperature_min << "°C - " << config_.temperature_max << "°C");
    PRINT_INFO("Update Interval: " << config_.update_interval_ms << "ms");
    
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
