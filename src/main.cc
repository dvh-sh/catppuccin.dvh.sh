/*
 * @file main.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * Main entry point for the Catppuccin API server.
 */
#include "catppuccin_api.hpp"
#include "config.hpp"
#include <iostream>
#include <signal.h>

// Global server instance for graceful shutdown
httplib::Server *g_server = nullptr;

/*
 * @brief Signal handler for graceful shutdown.
 */
void signalHandler(int signal)
{
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    if (g_server)
    {
        g_server->stop();
    }
}

/*
 * @brief Main function with optimized server configuration.
 */
int main()
{
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try
    {
        Catppuccin::CatppuccinAPI api;
        httplib::Server server;
        g_server = &server;

        // Performance optimizations
        server.set_keep_alive_max_count(100);
        server.set_keep_alive_timeout(30);
        server.set_read_timeout(5, 0);
        server.set_write_timeout(5, 0);
        server.set_idle_interval(0, 100000);        // 100ms
        server.set_payload_max_length(1024 * 1024); // 1MB max

        api.setupRoutes(server);

        std::string host = Catppuccin::Config::getHost();
        int port = Catppuccin::Config::getPort();

        std::cout << "Catppuccin API starting on http://" << host << ":" << port << std::endl;
        std::cout << "Config: " << Catppuccin::Config::getThreads() << " threads, "
                  << Catppuccin::Config::getRateLimit() << " req/hour limit" << std::endl;
        std::cout << "Available endpoints:" << std::endl;
        std::cout << "  GET /health                           - Health check" << std::endl;
        std::cout << "  GET /ports?page=1&per_page=20         - List ports" << std::endl;
        std::cout << "  GET /ports/:identifier                - Get specific port" << std::endl;
        std::cout << "  GET /collaborators?page=1&per_page=20 - List collaborators" << std::endl;
        std::cout << "  GET /categories?page=1&per_page=20    - List categories" << std::endl;
        std::cout << "  GET /showcases?page=1&per_page=20     - List showcases" << std::endl;
        std::cout << "  GET /userstyles?page=1&per_page=20    - List userstyles" << std::endl;
        std::cout << "  GET /userstyles/:identifier           - Get specific userstyle" << std::endl;
        std::cout << "  GET /palette                          - Color palette" << std::endl;
        std::cout << "  GET /metrics                          - Performance metrics" << std::endl;
        std::cout << std::endl;

        // Start server with thread pool
        if (!server.listen(host, port, Catppuccin::Config::getThreads()))
        {
            std::cerr << "Failed to start server on " << host << ":" << port << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server stopped" << std::endl;
    return 0;
}
