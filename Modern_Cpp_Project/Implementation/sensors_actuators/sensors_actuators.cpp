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
    
    // Initialize slider with config values
    // Multiply by 10 to allow 0.1°C precision with integer slider
    ui->sliderTemperature->setMinimum(static_cast<int>(config_.temperature_min * 10));
    ui->sliderTemperature->setMaximum(static_cast<int>(config_.temperature_max * 10));
    ui->sliderTemperature->setValue(static_cast<int>((config_.temperature_min + config_.temperature_max) / 2 * 10));
    
    // Update min/max labels
    ui->labelMinTemp->setText(QString::number(config_.temperature_min, 'f', 1) + "°C");
    ui->labelMaxTemp->setText(QString::number(config_.temperature_max, 'f', 1) + "°C");
    
    // Connect slider to temperature display update
    connect(ui->sliderTemperature, &QSlider::valueChanged, this, &sensors_actuators::onTemperatureChanged);
    
    // Initialize temperature display
    onTemperatureChanged(ui->sliderTemperature->value());
    
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
// SLOTS
// ===================================================================
void sensors_actuators::onTemperatureChanged(int value)
{
    // Convert slider value back to temperature (divide by 10)
    float temperature = value / 10.0f;
    
    // Update temperature display
    ui->labelTemperatureValue->setText(QString::number(temperature, 'f', 1) + "°C");
    
    PRINT_DEBUG("Temperature changed: " << temperature << "°C");
}

// ===================================================================
// END OF FILE
// ===================================================================
