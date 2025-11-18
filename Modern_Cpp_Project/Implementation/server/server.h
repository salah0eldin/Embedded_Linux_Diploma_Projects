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
#include <QTimer>
#include <thread>
#include <memory>
#include <atomic>
#include "TCPSocket.hpp"
#include "UDPSocket.hpp"
#include "ServerConfigParser.hpp"

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
     * @param configFile Path to YAML configuration file
     * @param parent Parent widget (default: nullptr)
     */
    server(const QString& configFile, QWidget *parent = nullptr);
    
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
    void onThresholdChanged(int value);
    void updateTemperatureDisplay(float temperature);
    void updateLedStatus(bool ledOn);

// ===================================================================
// PRIVATE METHODS
// ===================================================================
private:
    void startServer();
    void stopServer();
    void tcpListenerThread();
    void udpReceiverThread();
    void sendThresholdToClient(float threshold);

// ===================================================================
// PRIVATE MEMBERS
// ===================================================================
private:
    Ui::server *ui;
    
    // Configuration
    ServerConfig config_;
    
    // Socket members
    std::unique_ptr<TCPSocket> tcp_socket_;
    std::unique_ptr<UDPSocket> udp_socket_;
    
    // Threading
    std::unique_ptr<std::thread> tcp_thread_;
    std::unique_ptr<std::thread> udp_thread_;
    std::atomic<bool> running_;
    
    // State
    float current_threshold_;
    float current_temperature_;
    bool current_led_state_;
};

// ===================================================================
// END OF FILE
// ===================================================================
