/*
 * @file rate_limiter.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file implements the RateLimiter class, providing functionality
 * to manage and enforce rate limits on incoming requests based on client IP addresses.
 */
#include "rate_limiter.hpp"

#include <iostream>

namespace Catppuccin
{
/*
 * @brief Constructs a RateLimiter object.
 * @param {int} max_requests_per_window - The maximum number of requests allowed within a window.
 * @param {int} window_seconds - The duration of the rate limit window in seconds.
 */
RateLimiter::RateLimiter(int max_requests_per_window, int window_seconds)
    : max_requests_(max_requests_per_window),
      window_duration_(window_seconds),
      last_cleanup_(std::chrono::steady_clock::now())
{
}

/*
 * @brief Checks if a client's rate limit window has expired.
 * @param {const ClientInfo&} info - The client's rate limit information.
 * @returns {bool} True if the window has expired, false otherwise.
 */
bool RateLimiter::isWindowExpired(const ClientInfo &info)
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - info.window_start);
    return elapsed >= window_duration_;
}

/*
 * @brief Resets a client's rate limit window.
 * @param {ClientInfo&} info - The client's rate limit information to reset.
 */
void RateLimiter::resetClientWindow(ClientInfo &info)
{
    info.request_count = 0;
    info.window_start = std::chrono::steady_clock::now();
}

/*
 * @brief Checks if a client is allowed to make a request.
 * @param {const std::string&} client_ip - The IP address of the client.
 * @returns {bool} True if the request is allowed, false otherwise (rate limit exceeded).
 */
bool RateLimiter::isAllowed(const std::string &client_ip)
{
    auto now = std::chrono::steady_clock::now();

    // Cleanup old entries periodically
    if (std::chrono::duration_cast<std::chrono::minutes>(now - last_cleanup_).count() >= 5)
    {
        cleanup();
        last_cleanup_ = now;
    }

    // Allow localhost with higher limits for development
    if (client_ip == "127.0.0.1" || client_ip == "::1" || client_ip == "localhost")
    {
        return true;
    }

    ClientInfo &client = clients_[client_ip];

    if (isWindowExpired(client))
    {
        resetClientWindow(client);
    }

    if (client.request_count >= max_requests_)
    {
        std::cout << "Rate limit exceeded for IP: " << client_ip << " (" << client.request_count << "/" << max_requests_
                  << ")" << std::endl;
        return false;
    }

    client.request_count++;
    return true;
}

/*
 * @brief Retrieves detailed rate limit information for a specific client.
 * @param {const std::string&} client_ip - The IP address of the client.
 * @returns {RateLimitInfo} A struct containing rate limit details.
 */
RateLimitInfo RateLimiter::getRateLimitInfo(const std::string &client_ip)
{
    RateLimitInfo info;
    info.limit = max_requests_;

    if (client_ip == "127.0.0.1" || client_ip == "::1" || client_ip == "localhost")
    {
        info.requests_made = 0;
        info.requests_remaining = max_requests_ * 10;  // 10x limit for localhost
        info.reset_time = std::chrono::seconds(0);
        return info;
    }

    auto it = clients_.find(client_ip);
    if (it == clients_.end())
    {
        info.requests_made = 0;
        info.requests_remaining = max_requests_;
        info.reset_time = window_duration_;
    } else
    {
        const ClientInfo &client = it->second;
        info.requests_made = client.request_count;
        info.requests_remaining = max_requests_ - client.request_count;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - client.window_start);
        info.reset_time = window_duration_ - elapsed;

        if (info.reset_time.count() < 0)
        {
            info.reset_time = std::chrono::seconds(0);
        }
    }

    return info;
}

/*
 * @brief Cleans up old, expired client entries from the rate limiter.
 */
void RateLimiter::cleanup()
{
    auto now = std::chrono::steady_clock::now();
    auto it = clients_.begin();

    while (it != clients_.end())
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.window_start);

        // Remove entries older than 2 windows
        if (elapsed >= (window_duration_ * 2))
        {
            std::cout << "Cleaning up old rate limit entry for IP: " << it->first << std::endl;
            it = clients_.erase(it);
        } else
        {
            ++it;
        }
    }
}
}  // namespace Catppuccin
