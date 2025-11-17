// ===================================================================
// FILE: server.h
// DESCRIPTION: Server main window class declaration
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include <QMainWindow>
#include <QDesktopServices>
#include <QUrl>

// ===================================================================
// FORWARD DECLARATIONS
// ===================================================================
QT_BEGIN_NAMESPACE
namespace Ui {
class server;
}
QT_END_NAMESPACE

// ===================================================================
// SERVER CLASS
// ===================================================================
/**
 * @brief Main server window class
 * Handles GUI and server functionality
 */
class server : public QMainWindow {
    Q_OBJECT

// ===================================================================
// PUBLIC METHODS
// ===================================================================
public:
    /**
     * @brief Constructor
     * @param parent Parent widget (default: nullptr)
     */
    server(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~server();

// ===================================================================
// PRIVATE SLOTS
// ===================================================================
private slots:
    void onFacebookClicked();
    void onLinkedInClicked();
    void onInstagramClicked();

// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    Ui::server *ui;
};

// ===================================================================
// END OF FILE
// ===================================================================
