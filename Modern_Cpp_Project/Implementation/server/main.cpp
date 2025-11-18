// ===================================================================
// FILE: main.cpp
// DESCRIPTION: Server application main entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "server.h"
#include <QApplication>
// Logging config (defines g_current_log_level and macros)
#include "config.h"

// Define global log level for server (default INFO)
int g_current_log_level = LOG_LEVEL_INFO; // will be overridden by config parser if present

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Default config file path (relative to binary location)
    QString configFile = "../config.yaml";
    
    // Check if config file path provided as command-line argument
    if (argc > 1) {
        configFile = QString(argv[1]);
    }
    
    server w(configFile);
    w.show();
    return a.exec();
}

// ===================================================================
// END OF FILE
// ===================================================================
