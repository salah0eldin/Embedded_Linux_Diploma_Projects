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
    std::string tcp_ip = "0.0.0.0";
    int tcp_port = 9080;
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
            
            // Parse sensor_actuator_server section
            if (root.has_child("sensor_actuator_server")) {
                ryml::ConstNodeRef server = root["sensor_actuator_server"];
                
                if (server.has_child("tcp")) {
                    ryml::ConstNodeRef tcp = server["tcp"];
                    if (tcp.has_child("port")) {
                        tcp["port"] >> config.tcp_port;
                        PRINT_DEBUG("Parsed TCP port: " << config.tcp_port);
                    }
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
