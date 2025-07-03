/*
 * @file config.hpp
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * Configuration management for the Catppuccin API.
 * Handles environment variables and runtime settings.
 */
#ifndef CATPPUCCIN_CONFIG_HPP
#define CATPPUCCIN_CONFIG_HPP

#include <cstdlib>
#include <string>

namespace Catppuccin
{
/*
 * @class Config
 * @brief Manages application configuration from environment variables.
 */
class Config
{
public:
    // Server configuration
    static std::string getHost()
    {
        const char *env = std::getenv("HOST");
        return env ? env : "0.0.0.0";
    }

    static int getPort()
    {
        const char *env = std::getenv("PORT");
        return env ? std::atoi(env) : 3000;
    }

    static int getThreads()
    {
        const char *env = std::getenv("THREADS");
        return env ? std::atoi(env) : 4;
    }

    // Rate limiting
    static int getRateLimit()
    {
        const char *env = std::getenv("RATE_LIMIT");
        return env ? std::atoi(env) : 100000;  // 100,000 requests per hour
    }

    static int getRateWindow()
    {
        const char *env = std::getenv("RATE_WINDOW");
        return env ? std::atoi(env) : 3600;
    }

    // Cache settings
    static int getCacheTTL()
    {
        const char *env = std::getenv("CACHE_TTL");
        return env ? std::atoi(env) : 300;  // 5 minutes
    }

    // Logging
    static bool isVerbose()
    {
        const char *env = std::getenv("VERBOSE");
        return env && (std::string(env) == "true" || std::string(env) == "1");
    }
};
}  // namespace Catppuccin

#endif
