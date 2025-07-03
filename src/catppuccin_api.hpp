/*
 * @file catppuccin_api.hpp
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file defines the CatppuccinAPI class, which acts as the main
 * interface for the Catppuccin data API. It integrates data fetching,
 * rate limiting, and provides methods for various API endpoints.
 */
#ifndef CATPPUCCIN_API_HPP
#define CATPPUCCIN_API_HPP

#include "data_fetcher.hpp"
#include "rate_limiter.hpp"
#include "config.hpp"
#include "models.hpp"

#include "../include/httplib.hpp"

namespace Catppuccin
{
    /*
     * @class CatppuccinAPI
     * @brief Main class for the Catppuccin API, handling data access and server routes.
     *
     * @description
     * This class orchestrates the fetching and serving of Catppuccin-related data,
     * including ports, collaborators, categories, and the color palette. It also
     * incorporates rate limiting and pagination for API responses.
     */
    class CatppuccinAPI
    {
    private:
        DataFetcher fetcher_;      ///< Manages fetching and caching of external data.
        RateLimiter rate_limiter_; ///< Manages API request rate limits.

        std::atomic<uint64_t> request_count_{0};           ///< Total number of API requests received.
        std::atomic<uint64_t> error_count_{0};             ///< Total number of API errors encountered.
        std::chrono::steady_clock::time_point start_time_; ///< Start time of the API server.

        /*
         * @brief Paginate a JSON array based on pagination information.
         * @param {const json&} array - The JSON array to paginate.
         * @param {const PaginationInfo&} pagination - Pagination parameters.
         * @returns {json} A JSON array containing the paginated subset of items.
         */
        json paginateArray(const json &array, const PaginationInfo &pagination);

        /*
         * @brief Calculates pagination details for a given total number of items.
         * @param {int} total_items - The total number of items available.
         * @param {int} page - The requested page number.
         * @param {int} per_page - The number of items per page.
         * @returns {PaginationInfo} A struct containing calculated pagination details.
         */
        PaginationInfo calculatePagination(int total_items, int page, int per_page);

        /*
         * @brief Checks if the incoming request is within rate limits.
         * @param {const httplib::Request&} req - The incoming HTTP request.
         * @param {httplib::Response&} res - The HTTP response object.
         * @returns {bool} True if allowed, false if rate limit exceeded (and sets 429 response).
         */
        bool checkRateLimit(const httplib::Request &req, httplib::Response &res);

        /*
         * @brief Adds rate limit headers to the HTTP response.
         * @param {const httplib::Request&} req - The incoming HTTP request.
         * @param {httplib::Response&} res - The HTTP response object to modify.
         */
        void addRateLimitHeaders(const httplib::Request &req, httplib::Response &res);

    public:
        /*
         * @brief Constructs a CatppuccinAPI object and initializes components.
         */
        CatppuccinAPI();

        /*
         * @brief Destructor for CatppuccinAPI.
         */
        ~CatppuccinAPI();

        /*
         * @brief Retrieves a paginated list of Catppuccin ports.
         * @param {int} page - The page number to retrieve. Defaults to 1.
         * @param {int} per_page - The number of items per page. Defaults to 20.
         * @returns {ApiResponse} Response containing port data or an error.
         */
        ApiResponse getPorts(int page = 1, int per_page = 20);

        /*
         * @brief Retrieves a specific Catppuccin port by its identifier (key).
         * @param {const std::string&} identifier - The unique key of the port.
         * @returns {ApiResponse} Response containing port data or a "not found" error.
         */
        ApiResponse getPort(const std::string &identifier);

        /*
         * @brief Retrieves a paginated list of Catppuccin collaborators.
         * @param {int} page - The page number to retrieve. Defaults to 1.
         * @param {int} per_page - The number of items per page. Defaults to 20.
         * @returns {ApiResponse} Response containing collaborator data or an error.
         */
        ApiResponse getCollaborators(int page = 1, int per_page = 20);

        /*
         * @brief Retrieves a specific Catppuccin collaborator by username.
         * @param {const std::string&} username - The username of the collaborator.
         * @returns {ApiResponse} Response containing collaborator data or a "not found" error.
         */
        ApiResponse getCollaborator(const std::string &username);

        /*
         * @brief Retrieves a paginated list of Catppuccin categories.
         * @param {int} page - The page number to retrieve. Defaults to 1.
         * @param {int} per_page - The number of items per page. Defaults to 20.
         * @returns {ApiResponse} Response containing category data or an error.
         */
        ApiResponse getCategories(int page = 1, int per_page = 20);

        /*
         * @brief Retrieves a specific Catppuccin category by its key.
         * @param {const std::string&} key - The unique key of the category.
         * @returns {ApiResponse} Response containing category data or a "not found" error.
         */
        ApiResponse getCategory(const std::string &key);

        /*
         * @brief Retrieves a paginated list of Catppuccin showcases.
         * @param {int} page - The page number to retrieve. Defaults to 1.
         * @param {int} per_page - The number of items per page. Defaults to 20.
         * @returns {ApiResponse} Response containing showcase data or an error.
         */
        ApiResponse getShowcases(int page = 1, int per_page = 20);

        /*
         * @brief Retrieves the Catppuccin color palette.
         * @returns {ApiResponse} Response containing the color palette data.
         */
        ApiResponse getPalette();

        /*
         * @brief Get server metrics and statistics.
         * @returns {ApiResponse} Response containing metrics data such as request counts and error rates.
         */
        ApiResponse getMetrics();

        /*
         * @brief Sets up all API routes on the provided httplib server instance.
         * @param {httplib::Server&} server - The httplib server to configure.
         */
        void setupRoutes(httplib::Server &server);

        /*
         * @brief Clears all cached data.
         */
        void clearCache();

        /*
         * @brief Refreshes all cached data by clearing and re-fetching it.
         */
        void refreshCache();
    };
}

#endif
