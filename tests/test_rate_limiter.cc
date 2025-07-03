#include <chrono>
#include <thread>

#include "../include/catch2.hpp"
#include "../src/rate_limiter.hpp"

using namespace Catppuccin;

TEST_CASE("Rate limiter enforces limits correctly", "[rate_limiter]")
{
    SECTION("Allows requests under limit")
    {
        RateLimiter limiter(5, 10);  // 5 requests per 10 seconds

        REQUIRE(limiter.isAllowed("192.168.1.1") == true);
        REQUIRE(limiter.isAllowed("192.168.1.1") == true);
        REQUIRE(limiter.isAllowed("192.168.1.1") == true);
        REQUIRE(limiter.isAllowed("192.168.1.1") == true);
        REQUIRE(limiter.isAllowed("192.168.1.1") == true);
    }

    SECTION("Blocks requests over limit")
    {
        RateLimiter limiter(3, 10);  // 3 requests per 10 seconds

        REQUIRE(limiter.isAllowed("192.168.1.2") == true);
        REQUIRE(limiter.isAllowed("192.168.1.2") == true);
        REQUIRE(limiter.isAllowed("192.168.1.2") == true);
        REQUIRE(limiter.isAllowed("192.168.1.2") == false);  // Should be blocked
    }

    SECTION("Different IPs have separate limits")
    {
        RateLimiter limiter(2, 10);

        REQUIRE(limiter.isAllowed("192.168.1.3") == true);
        REQUIRE(limiter.isAllowed("192.168.1.3") == true);
        REQUIRE(limiter.isAllowed("192.168.1.3") == false);  // Blocked

        REQUIRE(limiter.isAllowed("192.168.1.4") == true);  // Different IP, should work
        REQUIRE(limiter.isAllowed("192.168.1.4") == true);
    }

    SECTION("Localhost is always allowed")
    {
        RateLimiter limiter(1, 10);  // Very restrictive

        REQUIRE(limiter.isAllowed("127.0.0.1") == true);
        REQUIRE(limiter.isAllowed("127.0.0.1") == true);
        REQUIRE(limiter.isAllowed("127.0.0.1") == true);  // Should always work
        REQUIRE(limiter.isAllowed("localhost") == true);
        REQUIRE(limiter.isAllowed("::1") == true);
    }

    SECTION("Rate limit info is accurate")
    {
        RateLimiter limiter(5, 3600);

        // Make some requests
        limiter.isAllowed("192.168.1.5");
        limiter.isAllowed("192.168.1.5");

        RateLimitInfo info = limiter.getRateLimitInfo("192.168.1.5");
        REQUIRE(info.limit == 5);
        REQUIRE(info.requests_made == 2);
        REQUIRE(info.requests_remaining == 3);
    }
}
