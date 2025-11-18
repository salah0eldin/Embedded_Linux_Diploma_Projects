// ===================================================================
// FILE: SensorsActuatorsConfigParser.hpp
// DESCRIPTION: Configuration parser for sensors_actuators application
// AUTHOR: Salah-Eldin Hassen
// DATE: November 18, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include "config.h"
#include <ryml.hpp>
#include <ryml_std.hpp>  // Required for to_csubstr(std::string)
#include <fstream>
#include <sstream>
#include <string>

// ===================================================================
// CONFIGURATION STRUCTURE
// ===================================================================
/**
 * @brief Configuration structure for sensors_actuators application
 */
struct SensorsActuatorsConfig {
    // Server connection settings
    std::string server_tcp_ip;
    int server_tcp_port;
    int server_udp_port;
    
    // Sensor settings
    float temperature_min;
    float temperature_max;
    int update_interval_ms;
    
    // Default values
    SensorsActuatorsConfig() 
        : server_tcp_ip("127.0.0.1"),
          server_tcp_port(8080),
          server_udp_port(8081),
          temperature_min(20.0f),
          temperature_max(35.0f),
          update_interval_ms(1000) {}
};

// ===================================================================
// CONFIGURATION PARSER CLASS
// ===================================================================
/**
 * @brief Parser for sensors_actuators YAML configuration file
 */
class SensorsActuatorsConfigParser {
public:
    /**
     * @brief Parse configuration from YAML file
     * @param config_file Path to YAML configuration file
     * @return SensorsActuatorsConfig Configuration structure
     */
    static SensorsActuatorsConfig parseConfig(const std::string& config_file) {
        SensorsActuatorsConfig config;
        
        try {
            // Read YAML file
            std::ifstream file(config_file);
            if (!file.is_open()) {
                PRINT_ERROR("Failed to open config file: " << config_file);
                PRINT_INFO("Using default configuration");
                return config;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string yaml_content = buffer.str();
            
            // Parse YAML
            ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_content));
            ryml::ConstNodeRef root = tree.rootref();
            
            // Parse server connection settings
            if (root.has_child("server")) {
                ryml::ConstNodeRef server = root["server"];
                
                if (server.has_child("tcp_ip")) {
                    std::string tcp_ip;
                    server["tcp_ip"] >> tcp_ip;
                    config.server_tcp_ip = tcp_ip;
                    PRINT_DEBUG("Parsed server TCP IP: " << config.server_tcp_ip);
                }
                
                if (server.has_child("tcp_port")) {
                    server["tcp_port"] >> config.server_tcp_port;
                    PRINT_DEBUG("Parsed server TCP port: " << config.server_tcp_port);
                }
                
                if (server.has_child("udp_port")) {
                    server["udp_port"] >> config.server_udp_port;
                    PRINT_DEBUG("Parsed server UDP port: " << config.server_udp_port);
                }
            }
            
            // Parse sensor settings
            if (root.has_child("sensors")) {
                ryml::ConstNodeRef sensors = root["sensors"];
                
                if (sensors.has_child("temperature_min")) {
                    sensors["temperature_min"] >> config.temperature_min;
                    PRINT_DEBUG("Parsed temperature_min: " << config.temperature_min);
                }
                
                if (sensors.has_child("temperature_max")) {
                    sensors["temperature_max"] >> config.temperature_max;
                    PRINT_DEBUG("Parsed temperature_max: " << config.temperature_max);
                }
                
                if (sensors.has_child("update_interval_ms")) {
                    sensors["update_interval_ms"] >> config.update_interval_ms;
                    PRINT_DEBUG("Parsed update_interval_ms: " << config.update_interval_ms);
                }
            }
            
            // Parse logging level
            if (root.has_child("logging")) {
                ryml::ConstNodeRef logging = root["logging"];
                
                if (logging.has_child("level")) {
                    std::string log_level;
                    logging["level"] >> log_level;
                    PRINT_DEBUG("Parsed log level from config: " << log_level);
                    setLogLevel(log_level);
                    PRINT_DEBUG("Log level set from config: " << log_level);
                }
            }
            
            PRINT_INFO("Configuration loaded successfully from: " << config_file);
            
        } catch (const std::exception& e) {
            PRINT_ERROR("Failed to parse config file: " << e.what());
            PRINT_INFO("Using default configuration");
        }
        
        return config;
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
