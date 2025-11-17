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

// ===================================================================
// CONSTRUCTOR
// ===================================================================
server::server(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::server) {
    ui->setupUi(this);
    
    // Connect social media buttons
    connect(ui->btnFacebook, &QPushButton::clicked, this, &server::onFacebookClicked);
    connect(ui->btnLinkedIn, &QPushButton::clicked, this, &server::onLinkedInClicked);
    connect(ui->btnInstagram, &QPushButton::clicked, this, &server::onInstagramClicked);
}

// ===================================================================
// DESTRUCTOR
// ===================================================================
server::~server() {
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
// END OF FILE
// ===================================================================
