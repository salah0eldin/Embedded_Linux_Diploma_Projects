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
// CONFIGURATION STRUCTURES
// ===================================================================
struct ServerConnectionConfig {
    std::string tcp_ip = "127.0.0.1";
    int tcp_port = 8080;
    std::string udp_ip = "127.0.0.1";
    int udp_port = 8081;
};

struct ClientConfig {
    ServerConnectionConfig host_server;
    ServerConnectionConfig sensor_actuator_server;
    std::string log_level = "debug";
    float initial_threshold = 25.0f;
    int check_interval = 100;
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
            
            // Parse host_server section
            if (root.has_child("host_server")) {
                ryml::ConstNodeRef host_server = root["host_server"];
                
                if (host_server.has_child("tcp")) {
                    ryml::ConstNodeRef tcp = host_server["tcp"];
                    if (tcp.has_child("ip")) {
                        std::string ip;
                        tcp["ip"] >> ip;
                        config.host_server.tcp_ip = ip;
                    }
                    if (tcp.has_child("port")) {
                        tcp["port"] >> config.host_server.tcp_port;
                    }
                }
                
                if (host_server.has_child("udp")) {
                    ryml::ConstNodeRef udp = host_server["udp"];
                    if (udp.has_child("ip")) {
                        std::string ip;
                        udp["ip"] >> ip;
                        config.host_server.udp_ip = ip;
                    }
                    if (udp.has_child("port")) {
                        udp["port"] >> config.host_server.udp_port;
                    }
                }
            }
            
            // Parse sensor_actuator_server section
            if (root.has_child("sensor_actuator_server")) {
                ryml::ConstNodeRef sensor_server = root["sensor_actuator_server"];
                
                if (sensor_server.has_child("tcp")) {
                    ryml::ConstNodeRef tcp = sensor_server["tcp"];
                    if (tcp.has_child("ip")) {
                        std::string ip;
                        tcp["ip"] >> ip;
                        config.sensor_actuator_server.tcp_ip = ip;
                    }
                    if (tcp.has_child("port")) {
                        tcp["port"] >> config.sensor_actuator_server.tcp_port;
                    }
                }
                
                if (sensor_server.has_child("udp")) {
                    ryml::ConstNodeRef udp = sensor_server["udp"];
                    if (udp.has_child("ip")) {
                        std::string ip;
                        udp["ip"] >> ip;
                        config.sensor_actuator_server.udp_ip = ip;
                    }
                    if (udp.has_child("port")) {
                        udp["port"] >> config.sensor_actuator_server.udp_port;
                    }
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
            
            // Parse application section
            if (root.has_child("application")) {
                ryml::ConstNodeRef application = root["application"];
                
                if (application.has_child("initial_threshold")) {
                    application["initial_threshold"] >> config.initial_threshold;
                }
                
                if (application.has_child("check_interval")) {
                    application["check_interval"] >> config.check_interval;
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
