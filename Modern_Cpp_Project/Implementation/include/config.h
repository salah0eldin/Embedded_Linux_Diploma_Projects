// ===================================================================
// FILE: config.h
// DESCRIPTION: Configuration file for logging system
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include <iostream>
#include <string>

// ===================================================================
// LOG LEVEL DEFINITIONS
// ===================================================================
/**
 * @brief Log Level Configuration
 * Log levels (higher number = more verbose):
 * - LOG_LEVEL_NONE  : No logging
 * - LOG_LEVEL_ERROR : Only error messages
 * - LOG_LEVEL_INFO  : Error and info messages
 * - LOG_LEVEL_DEBUG : Debug, info, and error messages
 * - LOG_LEVEL_TRACE : All messages including detailed trace (most verbose)
 */
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_TRACE 4

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Global variable to store current log level (can be changed at runtime)
extern int g_current_log_level;

// ===================================================================
// LOG LEVEL SETTER FUNCTIONS
// ===================================================================
/**
 * @brief Set the log level at runtime
 * @param level The log level (0-4)
 */
inline void setLogLevel(int level) {
    if (level >= LOG_LEVEL_NONE && level <= LOG_LEVEL_TRACE) {
        g_current_log_level = level;
    }
}

/**
 * @brief Set the log level from string
 * @param levelStr "none", "error", "info", "debug", or "trace"
 */
inline void setLogLevel(const std::string& levelStr) {
    if (levelStr == "none" || levelStr == "NONE") {
        g_current_log_level = LOG_LEVEL_NONE;
    } else if (levelStr == "error" || levelStr == "ERROR") {
        g_current_log_level = LOG_LEVEL_ERROR;
    } else if (levelStr == "info" || levelStr == "INFO") {
        g_current_log_level = LOG_LEVEL_INFO;
    } else if (levelStr == "debug" || levelStr == "DEBUG") {
        g_current_log_level = LOG_LEVEL_DEBUG;
    } else if (levelStr == "trace" || levelStr == "TRACE") {
        g_current_log_level = LOG_LEVEL_TRACE;
    }
}

// ===================================================================
// LOGGING MACROS
// ===================================================================
/**
 * @brief Trace print macro
 * Prints trace messages with [TRACE] prefix (most verbose)
 * Only active when current log level >= LOG_LEVEL_TRACE
 */
#define PRINT_TRACE(...) \
    do { \
        if (g_current_log_level >= LOG_LEVEL_TRACE) { \
            std::cout << "[TRACE] " << __VA_ARGS__ << std::endl; \
        } \
    } while(0)

/**
 * @brief Debug print macro
 * Prints debug messages with [DEBUG] prefix
 * Only active when current log level >= LOG_LEVEL_DEBUG
 */
#define PRINT_DEBUG(...) \
    do { \
        if (g_current_log_level >= LOG_LEVEL_DEBUG) { \
            std::cout << "[DEBUG] " << __VA_ARGS__ << std::endl; \
        } \
    } while(0)

/**
 * @brief Info print macro
 * Prints info messages with [INFO] prefix
 * Only active when current log level >= LOG_LEVEL_INFO
 */
#define PRINT_INFO(...) \
    do { \
        if (g_current_log_level >= LOG_LEVEL_INFO) { \
            std::cout << "[INFO] " << __VA_ARGS__ << std::endl; \
        } \
    } while(0)

/**
 * @brief Error print macro
 * Prints error messages with [ERROR] prefix to stderr
 * Only active when current log level >= LOG_LEVEL_ERROR
 */
#define PRINT_ERROR(...) \
    do { \
        if (g_current_log_level >= LOG_LEVEL_ERROR) { \
            std::cerr << "[ERROR] " << __VA_ARGS__ << std::endl; \
        } \
    } while(0)

// ===================================================================
// END OF FILE
// ===================================================================
