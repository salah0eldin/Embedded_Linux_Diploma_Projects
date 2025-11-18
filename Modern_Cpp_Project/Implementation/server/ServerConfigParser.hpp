// ===================================================================
// FILE: ServerConfigParser.hpp
// DESCRIPTION: YAML config parser for server settings using RapidYAML
// AUTHOR: Salah-Eldin Hassen
// DATE: November 17, 2025
// ===================================================================

#pragma once

// ===================================================================
// INCLUDES
// ===================================================================
// Workaround for Qt/RapidYAML conflict: undefine emit before including ryml
#ifdef emit
#undef emit
#define RYML_QT_EMIT_CONFLICT_WORKAROUND
#endif

#include <ryml.hpp>
#include <ryml_std.hpp>

// Redefine emit for Qt after ryml includes
#ifdef RYML_QT_EMIT_CONFLICT_WORKAROUND
#define emit
#undef RYML_QT_EMIT_CONFLICT_WORKAROUND
#endif

#include <string>
#include <fstream>
#include <sstream>

// ===================================================================
// CONFIGURATION STRUCTURE
// ===================================================================
struct ServerConfig {
    std::string tcp_ip = "0.0.0.0";
    int tcp_port = 8080;
    std::string udp_ip = "0.0.0.0";
    int udp_port = 8081;
    float initial_threshold = 30.0f;
};

// ===================================================================
// CONFIG PARSER CLASS
// ===================================================================
class ServerConfigParser {
public:
    /**
     * @brief Parse YAML configuration file using RapidYAML
     * @param filename Path to YAML file
     * @return ServerConfig structure with parsed values
     */
    static ServerConfig parseConfig(const std::string& filename) {
        ServerConfig config;
        
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
            
            // Parse host_server section (this is the server config from client's perspective)
            if (root.has_child("host_server")) {
                ryml::ConstNodeRef host_server = root["host_server"];
                
                if (host_server.has_child("tcp")) {
                    ryml::ConstNodeRef tcp = host_server["tcp"];
                    if (tcp.has_child("port")) {
                        tcp["port"] >> config.tcp_port;
                    }
                }
                
                if (host_server.has_child("udp")) {
                    ryml::ConstNodeRef udp = host_server["udp"];
                    if (udp.has_child("port")) {
                        udp["port"] >> config.udp_port;
                    }
                }
            }
            
            // Parse application section for initial threshold
            if (root.has_child("application")) {
                ryml::ConstNodeRef application = root["application"];
                
                if (application.has_child("initial_threshold")) {
                    application["initial_threshold"] >> config.initial_threshold;
                }
            }
            
        } catch (const std::exception& e) {
            // If parsing fails, return default config
        }
        
        return config;
    }
};

// ===================================================================
// END OF FILE
// ===================================================================
