// ===================================================================
// FILE: main.cpp
// DESCRIPTION: Sensors and Actuators application entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "sensors_actuators.h"
#include "config.h"
#include <QApplication>

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
    sensors_actuators w;
    w.show();
    return a.exec();
}

// ===================================================================
// END OF FILE
// ===================================================================
