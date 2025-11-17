// ===================================================================
// FILE: ConfigParser.hpp
// DESCRIPTION: YAML config parser for client settings using RapidYAML
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <string>
#include <fstream>
#include <sstream>

// ===================================================================
// CONFIGURATION STRUCTURE
// ===================================================================
struct ClientConfig {
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;
    std::string log_level = "debug";
    int connection_timeout = 5;
    int retry_attempts = 3;
};

// ===================================================================
// CONFIG PARSER CLASS
// ===================================================================
class ConfigParser {
public:
    /**
     * @brief Parse YAML configuration file using RapidYAML
     * @param filename Path to YAML file
     * @return ClientConfig structure with parsed values
     */
    static ClientConfig parseConfig(const std::string& filename) {
        ClientConfig config;
        
        try {
            // Read file contents
            std::ifstream file(filename);
            if (!file.is_open()) {
                // Return default config if file doesn't exist
                return config;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string yaml_content = buffer.str();
            file.close();
            
            // Parse YAML using RapidYAML
            ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_content));
            ryml::ConstNodeRef root = tree.rootref();
            
            // Parse server section
            if (root.has_child("server")) {
                ryml::ConstNodeRef server = root["server"];
                
                if (server.has_child("ip")) {
                    std::string ip;
                    server["ip"] >> ip;
                    config.server_ip = ip;
                }
                
                if (server.has_child("port")) {
                    server["port"] >> config.server_port;
                }
            }
            
            // Parse logging section
            if (root.has_child("logging")) {
                ryml::ConstNodeRef logging = root["logging"];
                
                if (logging.has_child("level")) {
                    std::string level;
                    logging["level"] >> level;
                    config.log_level = level;
                }
            }
            
            // Parse connection section
            if (root.has_child("connection")) {
                ryml::ConstNodeRef connection = root["connection"];
                
                if (connection.has_child("timeout")) {
                    connection["timeout"] >> config.connection_timeout;
                }
                
                if (connection.has_child("retry_attempts")) {
                    connection["retry_attempts"] >> config.retry_attempts;
                }
            }
            
        } catch (const std::exception& e) {
            // If parsing fails, return default config
            // Error will be logged by caller if needed
        }
        
        return config;
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
