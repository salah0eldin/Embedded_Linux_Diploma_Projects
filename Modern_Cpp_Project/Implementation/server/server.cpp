// ===================================================================
// FILE: server.cpp
// DESCRIPTION: Server main window class implementation
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

// ===================================================================
// INCLUDES
// ===================================================================
#include "server.h"
#include "./ui_server.h"
#include "config.h"
#include <QDebug>
#include <thread>

// ===================================================================
// CONSTRUCTOR
// ===================================================================
server::server(const QString& configFile, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::server),
      running_(false),
      current_threshold_(30.0f),
      current_temperature_(0.0f),
      current_led_state_(false),
      chart_(nullptr),
      chartView_(nullptr),
      series_(nullptr),
      axisX_(nullptr),
      axisY_(nullptr),
      chartUpdateTimer_(nullptr),
      dataPointCounter_(0) {
    ui->setupUi(this);
    
    // Load configuration from YAML file
    config_ = ServerConfigParser::parseConfig(configFile.toStdString());
    current_threshold_ = config_.initial_threshold;
    
    PRINT_INFO("Loaded configuration from: " << configFile.toStdString());
    PRINT_INFO("TCP Port: " << config_.tcp_port);
    PRINT_INFO("UDP Port: " << config_.udp_port);
    PRINT_INFO("Initial Threshold: " << config_.initial_threshold << "°C");
    
    // Setup chart
    setupChart();
    
    // Connect social media buttons
    connect(ui->btnFacebook, &QPushButton::clicked, this, &server::onFacebookClicked);
    connect(ui->btnLinkedIn, &QPushButton::clicked, this, &server::onLinkedInClicked);
    connect(ui->btnInstagram, &QPushButton::clicked, this, &server::onInstagramClicked);
    
    // Connect clear history button
    connect(ui->btnClearHistory, &QPushButton::clicked, this, &server::onClearHistoryClicked);
    
    // Connect threshold slider
    connect(ui->sliderThreshold, &QSlider::valueChanged,
            this, &server::onThresholdChanged);
    
    // Set initial threshold value in slider (multiply by 10 for 0.1 precision)
    ui->sliderThreshold->setMinimum(0);
    ui->sliderThreshold->setMaximum(1000);
    ui->sliderThreshold->setValue(static_cast<int>(current_threshold_ * 10));
    
    // Update threshold label
    ui->labelThresholdValue->setText(QString::number(current_threshold_, 'f', 1) + "°C");
    
    // Start the server
    startServer();
}

// ===================================================================
// DESTRUCTOR
// ===================================================================
server::~server() {
    if (chartUpdateTimer_) {
        chartUpdateTimer_->stop();
        delete chartUpdateTimer_;
    }
    stopServer();
    delete ui;
}

// ===================================================================
// SLOT: FACEBOOK CLICKED
// ===================================================================
void server::onFacebookClicked() {
    QDesktopServices::openUrl(QUrl("https://www.facebook.com"));
}

// ===================================================================
// SLOT: LINKEDIN CLICKED
// ===================================================================
void server::onLinkedInClicked() {
    QDesktopServices::openUrl(QUrl("https://www.linkedin.com"));
}

// ===================================================================
// SLOT: INSTAGRAM CLICKED
// ===================================================================
void server::onInstagramClicked() {
    QDesktopServices::openUrl(QUrl("https://www.instagram.com"));
}

// ===================================================================
// SLOT: HANDLE THRESHOLD CHANGES
// ===================================================================
void server::onThresholdChanged(int value) {
    // Convert slider value to temperature (divide by 10)
    current_threshold_ = value / 10.0f;
    
    // Update threshold label
    ui->labelThresholdValue->setText(QString::number(current_threshold_, 'f', 1) + "°C");
    
    PRINT_INFO("Threshold changed to: " << current_threshold_ << "°C");
    sendThresholdToClient(current_threshold_);
}

// ===================================================================
// SLOT: UPDATE TEMPERATURE DISPLAY
// ===================================================================
void server::updateTemperatureDisplay(float temperature) {
    current_temperature_ = temperature;
    QString tempStr = QString::number(temperature, 'f', 1) + "°C";
    ui->labelTemperatureValue->setText(tempStr);
    PRINT_DEBUG("Temperature updated: " << tempStr.toStdString());
    
    // Add to temperature history for chart
    temperatureHistory_.push_back(temperature);
    
    // Keep history size reasonable (will be processed by updateChart timer)
    if (temperatureHistory_.size() > MAX_POINTS * 2) {
        temperatureHistory_.pop_front();
    }
}

// ===================================================================
// SLOT: UPDATE LED STATUS
// ===================================================================
void server::updateLedStatus(bool ledOn) {
    current_led_state_ = ledOn;
    
    QString ledText = ledOn ? "💡 LED Status: ON" : "💡 LED Status: OFF";
    QString ledColor = ledOn ? "color: #ffff00;" : "color: #e0e0e0;";
    
    ui->labelLedStatus->setText(ledText);
    ui->labelLedStatus->setStyleSheet(
        "QLabel {"
        "    background-color: rgba(0, 0, 0, 0.3);"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "    " + ledColor +
        "}"
    );
    
    PRINT_INFO("LED status updated: " << (ledOn ? "ON" : "OFF"));
}

// ===================================================================
// START SERVER
// ===================================================================
void server::startServer() {
    PRINT_INFO("Starting Host Server...");
    
    try {
        // Create TCP socket for threshold updates (server mode)
        tcp_socket_ = std::make_unique<TCPSocket>(config_.tcp_port, config_.tcp_ip, true);
        
        // Create UDP socket for receiving temperature (bind immediately)
        udp_socket_ = std::make_unique<UDPSocket>(config_.udp_ip, config_.udp_port, true);
        
        running_ = true;
        
        // Start TCP listener thread (std::thread) so we can join on shutdown
        tcp_thread_ = std::make_unique<std::thread>(&server::tcpListenerThread, this);

        // Start UDP receiver thread
        udp_thread_ = std::make_unique<std::thread>(&server::udpReceiverThread, this);
        
    PRINT_INFO("Host Server started successfully");
    PRINT_INFO("TCP Server listening on port " << config_.tcp_port);
    PRINT_INFO("UDP Server listening on port " << config_.udp_port);
        
    } catch (const std::exception& e) {
        PRINT_ERROR("Failed to start server: " << e.what());
    }
}

// ===================================================================
// STOP SERVER
// ===================================================================
void server::stopServer() {
    if (!running_) {
        return;
    }
    
    PRINT_INFO("Stopping Host Server...");
    running_ = false;
    
    // Close sockets
    if (tcp_socket_) {
        tcp_socket_->shutdown();
    }
    PRINT_INFO("TCP socket shut down");
    if (udp_socket_) {
        udp_socket_->shutdown();
    }
    PRINT_INFO("UDP socket shut down");
    
    // Wait for threads to finish: try to join with a short timeout, otherwise detach
    auto try_join_with_timeout = [](std::unique_ptr<std::thread>& thr, const char* name, int timeout_ms = 500) {
        if (!thr || !thr->joinable()) return;
        using clk = std::chrono::steady_clock;
        auto start = clk::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(clk::now() - start).count() < timeout_ms) {
            // If thread finished, join and return
            // There's no direct way to check if thread finished without join; try a small sleep and then attempt join using try/catch
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // Attempt to join if not joinable will be no-op, so check joinable again
            if (!thr->joinable()) return;
        }
        // If still running, detach to allow process to exit; log a warning
        PRINT_ERROR(std::string("Thread '") + name + " did not stop in time - detaching");
        try {
            thr->detach();
        } catch (...) {
            // ignore
        }
    };

    try_join_with_timeout(tcp_thread_, "TCP Thread");
    try_join_with_timeout(udp_thread_, "UDP Thread");
    
    PRINT_INFO("Host Server stopped");
}

// ===================================================================
// TCP LISTENER THREAD
// ===================================================================
void server::tcpListenerThread() {
    PRINT_INFO("[TCP Thread] Started - Waiting for client connection...");
    
    // Wait for client to connect
    if (!tcp_socket_->waitForConnect()) {
        PRINT_ERROR("[TCP Thread] Failed to accept client connection");
        return;
    }
    PRINT_INFO("[TCP Thread] Client connected");
    
    // Send initial threshold to client
    sendThresholdToClient(current_threshold_);
    
    // Keep connection alive (client will receive threshold updates)
    char buffer[1024];
    while (running_) {
        int bytes = tcp_socket_->receive(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string message(buffer);
            PRINT_DEBUG("[TCP Thread] Received: " << message);
            
            // Parse LED status updates from client
            if (message.find("LED:") == 0) {
                bool ledOn = (message.find("LED:ON") == 0);
                QMetaObject::invokeMethod(this, [this, ledOn]() {
                    updateLedStatus(ledOn);
                }, Qt::QueuedConnection);
            }
        } else if (bytes == 0 || bytes == -1) {
            PRINT_INFO("[TCP Thread] Client disconnected");
            break;
        }
    }
    PRINT_INFO("[TCP Thread] Stopped");
}

// ===================================================================
// UDP RECEIVER THREAD
// ===================================================================
void server::udpReceiverThread() {
    PRINT_INFO("[UDP Thread] Started - Listening for temperature data...");
    
    char buffer[1024];
    while (running_) {
        int bytes = udp_socket_->receive(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string message(buffer);
            
            // Parse temperature (format: "TEMP:25.5")
            if (message.find("TEMP:") == 0) {
                try {
                    float temperature = std::stof(message.substr(5));
                    PRINT_DEBUG("[UDP Thread] Received temperature: " << temperature << "°C");
                    
                    // Update GUI from main thread
                    QMetaObject::invokeMethod(this, [this, temperature]() {
                        updateTemperatureDisplay(temperature);
                    }, Qt::QueuedConnection);
                    
                } catch (const std::exception& e) {
                    PRINT_ERROR("[UDP Thread] Failed to parse temperature: " << e.what());
                }
            }
        } else if (bytes == -1) {
            running_ = false;
            break;
        }
        
        // Small sleep to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // UDP thread exiting
    PRINT_INFO("[UDP Thread] Stopped");

}
// ===================================================================
// SEND THRESHOLD TO CLIENT
// ===================================================================
void server::sendThresholdToClient(float threshold) {
    if (!tcp_socket_) {
        PRINT_ERROR("TCP socket not initialized");
        return;
    }
    
    std::string message = "THRESHOLD:" + std::to_string(threshold);
    int sent = tcp_socket_->send(message);
    
    if (sent > 0) {
        PRINT_INFO("Sent threshold to client: " << threshold << "°C");
    } else {
        PRINT_ERROR("Failed to send threshold to client");
    }
}

// ===================================================================
// SETUP CHART
// ===================================================================
void server::setupChart() {
    PRINT_INFO("Setting up temperature history chart...");
    
    // Create chart
    chart_ = new QChart();
    chart_->setTitle("Real-Time Temperature History");
    chart_->setAnimationOptions(QChart::NoAnimation);
    chart_->setTheme(QChart::ChartThemeLight);
    
    // Create series
    series_ = new QLineSeries();
    QPen pen(QColor(27, 133, 193));  // Blue color matching the theme
    pen.setWidth(2);
    series_->setPen(pen);
    chart_->addSeries(series_);
    
    // Create X axis (time)
    axisX_ = new QValueAxis();
    axisX_->setTitleText("Time (data points)");
    axisX_->setLabelFormat("%d");
    axisX_->setRange(0, MAX_POINTS);
    chart_->addAxis(axisX_, Qt::AlignBottom);
    series_->attachAxis(axisX_);
    
    // Create Y axis (temperature)
    axisY_ = new QValueAxis();
    axisY_->setTitleText("Temperature (°C)");
    axisY_->setLabelFormat("%.1f");
    axisY_->setRange(0, 50);  // 0 to 50°C range
    chart_->addAxis(axisY_, Qt::AlignLeft);
    series_->attachAxis(axisY_);
    
    // Create chart view
    chartView_ = new QChartView(chart_);
    chartView_->setRenderHint(QPainter::Antialiasing);
    
    // Add chart view to the chartWidget
    QVBoxLayout* layout = new QVBoxLayout(ui->chartWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView_);
    ui->chartWidget->setLayout(layout);
    
    // Create timer for chart updates (10x slower than real data)
    chartUpdateTimer_ = new QTimer(this);
    connect(chartUpdateTimer_, &QTimer::timeout, this, &server::updateChart);
    chartUpdateTimer_->start(UPDATE_INTERVAL_MS);
    
    PRINT_INFO("Chart setup complete");
}

// ===================================================================
// UPDATE CHART
// ===================================================================
void server::updateChart() {
    // Check if we have unprocessed data
    // The history buffer contains all received temperatures
    // We plot one point per timer tick to create the smooth scrolling effect
    
    if (temperatureHistory_.empty()) {
        return;
    }
    
    // Take and remove the first (oldest) temperature from history
    float temp = temperatureHistory_.front();
    temperatureHistory_.pop_front();
    
    // Add point to series
    series_->append(dataPointCounter_, temp);
    dataPointCounter_++;
    
    // Keep only last MAX_POINTS points in the chart
    if (series_->count() > MAX_POINTS) {
        series_->remove(0);
        
        // Update X axis range to show scrolling effect
        qreal minX = series_->at(0).x();
        qreal maxX = minX + MAX_POINTS;
        axisX_->setRange(minX, maxX);
    }
    
    // Auto-adjust Y axis range based on data
    if (series_->count() > 0) {
        qreal minY = 100, maxY = 0;
        for (const QPointF& point : series_->points()) {
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }
        
        // Add some padding
        qreal padding = (maxY - minY) * 0.2;
        if (padding < 5) padding = 5;  // Minimum padding
        axisY_->setRange(minY - padding, maxY + padding);
    }
}

// ===================================================================
// SLOT: CLEAR HISTORY
// ===================================================================
void server::onClearHistoryClicked() {
    PRINT_INFO("Clearing temperature history...");
    
    series_->clear();
    temperatureHistory_.clear();
    dataPointCounter_ = 0;
    
    // Reset axes
    axisX_->setRange(0, MAX_POINTS);
    axisY_->setRange(0, 50);
    
    PRINT_INFO("History cleared");
}

// ===================================================================
// UPDATE TEMPERATURE DISPLAY
// ===================================================================
// ===================================================================