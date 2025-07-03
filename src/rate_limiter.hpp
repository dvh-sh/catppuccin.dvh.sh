/*
 * @file rate_limiter.hpp
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file defines the RateLimitInfo struct and the RateLimiter class,
 * which is used to control and limit the rate of requests from clients
 * based on their IP address.
 */
#ifndef RATE_LIMITER_HPP
#define RATE_LIMITER_HPP

#include <chrono>
#include <map>
#include <string>

namespace Catppuccin
{
/*
 * @struct RateLimitInfo
 * @brief Holds current rate limiting information for a client.
 */
struct RateLimitInfo
{
    int requests_made;                ///< Number of requests made in the current window.
    int requests_remaining;           ///< Number of requests remaining until the limit.
    int limit;                        ///< The total request limit for the window.
    std::chrono::seconds reset_time;  ///< Time until the rate limit resets.
};

/*
 * @class RateLimiter
 * @brief Manages request rate limiting for clients based on their IP.
 */
class RateLimiter
{
private:
    /*
     * @struct ClientInfo
     * @brief Internal structure to store rate limiting state for each client.
     */
    struct ClientInfo
    {
        int request_count;                                   ///< Number of requests from this client.
        std::chrono::steady_clock::time_point window_start;  ///< Start time of the current rate limit window.

        ClientInfo() : request_count(0), window_start(std::chrono::steady_clock::now())
        {
        }
    };

    std::map<std::string, ClientInfo> clients_;           ///< Map of client IP addresses to their rate limit info.
    int max_requests_;                                    ///< Maximum allowed requests per window.
    std::chrono::seconds window_duration_;                ///< Duration of the rate limit window.
    std::chrono::steady_clock::time_point last_cleanup_;  ///< Last time old client entries were cleaned up.

public:
    /*
     * @brief Constructs a RateLimiter object.
     * @param {int} max_requests_per_window - The maximum number of requests allowed within a window.
     * @param {int} window_seconds - The duration of the rate limit window in seconds.
     */
    RateLimiter(int max_requests_per_window, int window_seconds);

    /*
     * @brief Checks if a client is allowed to make a request.
     * @param {const std::string&} client_ip - The IP address of the client.
     * @returns {bool} True if the request is allowed, false otherwise (rate limit exceeded).
     */
    bool isAllowed(const std::string &client_ip);

    /*
     * @brief Retrieves detailed rate limit information for a specific client.
     * @param {const std::string&} client_ip - The IP address of the client.
     * @returns {RateLimitInfo} A struct containing rate limit details.
     */
    RateLimitInfo getRateLimitInfo(const std::string &client_ip);

    /*
     * @brief Cleans up old, expired client entries from the rate limiter.
     */
    void cleanup();

private:
    /*
     * @brief Checks if a client's rate limit window has expired.
     * @param {const ClientInfo&} info - The client's rate limit information.
     * @returns {bool} True if the window has expired, false otherwise.
     */
    bool isWindowExpired(const ClientInfo &info);

    /*
     * @brief Resets a client's rate limit window.
     * @param {ClientInfo&} info - The client's rate limit information to reset.
     */
    void resetClientWindow(ClientInfo &info);
};
}  // namespace Catppuccin

#endif
