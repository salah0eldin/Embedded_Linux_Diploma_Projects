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
