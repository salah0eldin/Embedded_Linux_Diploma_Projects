// ===================================================================
// FILE: main.cpp
// DESCRIPTION: Server application main entry point
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "ServerConfigParser.hpp"
#include "config.h"

#include "server.h"
#include <QApplication>
#include <QString>

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Define global log level for server (default INFO)
int g_current_log_level = LOG_LEVEL_INFO; // will be overridden by config parser if present

// ===================================================================
// MAIN FUNCTION
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
    
    PRINT_INFO("Starting Server Application...");

    server w(configFile);
    w.show();

    PRINT_INFO("Application window shown successfully");

    return a.exec();
}

// ===================================================================
// END OF FILE
// ===================================================================
