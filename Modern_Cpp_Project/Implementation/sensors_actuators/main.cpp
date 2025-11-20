// ===================================================================
// FILE: main.cpp
// DESCRIPTION: Sensors and Actuators application entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "SensorsActuatorsConfigParser.hpp"
#include "config.h"

#include "sensors_actuators.h"
#include <QApplication>
#include <QString>

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Define global log level (used by PRINT_* macros in config.h)
int g_current_log_level = LOG_LEVEL_INFO;

// ===================================================================
// MAIN ENTRY POINT
// ===================================================================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Determine config file path
    QString configFile;
    if (argc > 1) {
        configFile = QString(argv[1]);
        PRINT_INFO("Using config file from command line: " << configFile.toStdString());
    } else {
        // Default config file path (relative to executable)
        configFile = "../config.yaml";
        PRINT_INFO("Using default config file: " << configFile.toStdString());
    }
    
    PRINT_INFO("Starting Sensors & Actuators Application...");
    
    sensors_actuators w(configFile);
    w.show();
    
    PRINT_INFO("Application window shown successfully");
    
    return a.exec();
}

// ===================================================================
// END OF FILE
// ===================================================================
