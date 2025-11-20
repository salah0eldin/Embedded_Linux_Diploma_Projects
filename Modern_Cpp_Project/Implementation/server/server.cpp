// ===================================================================
// FILE: server.cpp
// DESCRIPTION: Server GUI implementation (charts, buttons, slots)
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
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &server::onThresholdChanged);
    
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
    QDesktopServices::openUrl(QUrl("https://www.linkedin.com/in/salah-eldin-hassen-5bba10250/"));
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
// END OF FILE
// ===================================================================