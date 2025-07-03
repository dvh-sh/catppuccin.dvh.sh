/*
 * @file catppuccin_api.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file implements the CatppuccinAPI class, providing the core
 * logic for handling various API endpoints, including data retrieval,
 * pagination, and rate limiting.
 */
#include "catppuccin_api.hpp"
#include <iostream>
#include <algorithm>

namespace Catppuccin
{
    /*
     * @brief Constructs a CatppuccinAPI object and initializes components.
     */
    CatppuccinAPI::CatppuccinAPI() : rate_limiter_(Config::getRateLimit(), Config::getRateWindow()),
                                     start_time_(std::chrono::steady_clock::now()) {}
    /*
     * @brief Destructor for CatppuccinAPI.
     */
    CatppuccinAPI::~CatppuccinAPI() {}

    /*
     * @brief Checks if the incoming request is within rate limits.
     * @param {const httplib::Request&} req - The incoming HTTP request.
     * @param {httplib::Response&} res - The HTTP response object.
     * @returns {bool} True if allowed, false if rate limit exceeded (and sets 429 response).
     */
    bool CatppuccinAPI::checkRateLimit(const httplib::Request &req, httplib::Response &res)
    {
        std::string client_ip = req.remote_addr;

        if (!rate_limiter_.isAllowed(client_ip))
        {
            res.status = 429; // Too Many Requests
            res.set_content(json::object({{"error", "Rate limit exceeded"},
                                          {"message", "Too many requests. Please try again later."},
                                          {"status", 429}})
                                .dump(2),
                            "application/json");
            return false;
        }
        return true;
    }

    /*
     * @brief Adds rate limit headers to the HTTP response.
     * @param {const httplib::Request&} req - The incoming HTTP request.
     * @param {httplib::Response&} res - The HTTP response object to modify.
     */
    void CatppuccinAPI::addRateLimitHeaders(const httplib::Request &req, httplib::Response &res)
    {
        std::string client_ip = req.remote_addr;
        RateLimitInfo info = rate_limiter_.getRateLimitInfo(client_ip);

        res.set_header("X-RateLimit-Limit", std::to_string(info.limit));
        res.set_header("X-RateLimit-Remaining", std::to_string(info.requests_remaining));
        res.set_header("X-RateLimit-Reset", std::to_string(info.reset_time.count()));
        res.set_header("X-RateLimit-Used", std::to_string(info.requests_made));
    }

    /*
     * @brief Calculates pagination details for a given total number of items.
     * @param {int} total_items - The total number of items available.
     * @param {int} page - The requested page number.
     * @param {int} per_page - The number of items per page.
     * @returns {PaginationInfo} A struct containing calculated pagination details.
     */
    PaginationInfo CatppuccinAPI::calculatePagination(int total_items, int page, int per_page)
    {
        PaginationInfo pagination(page, per_page);
        pagination.total_items = total_items;
        pagination.total_pages = (total_items + per_page - 1) / per_page;
        return pagination;
    }

    /*
     * @brief Paginate a JSON array based on pagination information.
     * @param {const json&} array - The JSON array to paginate.
     * @param {const PaginationInfo&} pagination - Pagination parameters.
     * @returns {json} A JSON array containing the paginated subset of items.
     */
    json CatppuccinAPI::paginateArray(const json &array, const PaginationInfo &pagination)
    {
        if (!array.is_array())
            return json::array();

        int start = (pagination.page - 1) * pagination.per_page;
        int end = start + pagination.per_page;

        json result = json::array();
        for (int i = start; i < end && i < (int)array.size(); ++i)
        {
            result.push_back(array[i]);
        }
        return result;
    }

    /*
     * @brief Retrieves a paginated list of Catppuccin ports.
     * @param {int} page - The page number to retrieve. Defaults to 1.
     * @param {int} per_page - The number of items per page. Defaults to 20.
     * @returns {ApiResponse} Response containing port data or an error.
     */
    ApiResponse CatppuccinAPI::getPorts(int page, int per_page)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(PORTS))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(PORTS);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(PORTS);
            json all_ports = json::array();

            if (data.contains("ports") && data["ports"].is_array())
            {
                for (const auto &port : data["ports"])
                {
                    json p = port;
                    p["is-userstyle"] = false;
                    p["is-archived"] = false;
                    all_ports.push_back(p);
                }
            }

            if (data.contains("archived-ports") && data["archived-ports"].is_array())
            {
                for (const auto &port : data["archived-ports"])
                {
                    json p = port;
                    p["is-userstyle"] = false;
                    p["is-archived"] = true;
                    all_ports.push_back(p);
                }
            }

            PaginationInfo pagination = calculatePagination(all_ports.size(), page, per_page);
            json paginated_data = paginateArray(all_ports, pagination);

            response.data = json::object();
            response.data["ports"] = paginated_data;
            response.data["pagination"] = {
                {"page", pagination.page},
                {"per_page", pagination.per_page},
                {"total_items", pagination.total_items},
                {"total_pages", pagination.total_pages}};
            response.success = true;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error processing ports: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a specific Catppuccin port by its identifier (key).
     * @param {const std::string&} identifier - The unique key of the port.
     * @returns {ApiResponse} Response containing port data or a "not found" error.
     */
    ApiResponse CatppuccinAPI::getPort(const std::string &identifier)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(PORTS))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(PORTS);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(PORTS);

            if (data.contains("ports") && data["ports"].is_array())
            {
                for (const auto &port : data["ports"])
                {
                    if (port.contains("key") && port["key"] == identifier)
                    {
                        response.data = port;
                        response.data["is-archived"] = false;
                        response.success = true;
                        return response;
                    }
                }
            }

            if (data.contains("archived-ports") && data["archived-ports"].is_array())
            {
                for (const auto &port : data["archived-ports"])
                {
                    if (port.contains("key") && port["key"] == identifier)
                    {
                        response.data = port;
                        response.data["is-archived"] = true;
                        response.success = true;
                        return response;
                    }
                }
            }

            response.error_message = "Port not found: " + identifier;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error finding port: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a paginated list of Catppuccin collaborators.
     * @param {int} page - The page number to retrieve. Defaults to 1.
     * @param {int} per_page - The number of items per page. Defaults to 20.
     * @returns {ApiResponse} Response containing collaborator data or an error.
     */
    ApiResponse CatppuccinAPI::getCollaborators(int page, int per_page)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(PORTS))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(PORTS);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(PORTS);

            if (data.contains("collaborators") && data["collaborators"].is_array())
            {
                const json &collaborators = data["collaborators"];

                PaginationInfo pagination = calculatePagination(collaborators.size(), page, per_page);
                json paginated_data = paginateArray(collaborators, pagination);

                response.data = json::object();
                response.data["collaborators"] = paginated_data;
                response.data["pagination"] = {
                    {"page", pagination.page},
                    {"per_page", pagination.per_page},
                    {"total_items", pagination.total_items},
                    {"total_pages", pagination.total_pages}};
                response.success = true;
            }
            else
            {
                response.error_message = "No collaborators found";
            }
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error processing collaborators: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a specific Catppuccin collaborator by username.
     * @param {const std::string&} username - The username of the collaborator.
     * @returns {ApiResponse} Response containing collaborator data or a "not found" error.
     */
    ApiResponse CatppuccinAPI::getCollaborator(const std::string &username)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(PORTS))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(PORTS);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(PORTS);

            if (data.contains("collaborators") && data["collaborators"].is_array())
            {
                for (const auto &collaborator : data["collaborators"])
                {
                    if (collaborator.contains("username") && collaborator["username"] == username)
                    {
                        response.data = collaborator;
                        response.success = true;
                        return response;
                    }
                }
            }

            response.error_message = "Collaborator not found: " + username;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error finding collaborator: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a paginated list of Catppuccin categories.
     * @param {int} page - The page number to retrieve. Defaults to 1.
     * @param {int} per_page - The number of items per page. Defaults to 20.
     * @returns {ApiResponse} Response containing category data or an error.
     */
    ApiResponse CatppuccinAPI::getCategories(int page, int per_page)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(CATEGORIES))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(CATEGORIES);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(CATEGORIES);
            json categories = data.is_array() ? data : json::array();

            PaginationInfo pagination = calculatePagination(categories.size(), page, per_page);
            json paginated_data = paginateArray(categories, pagination);

            response.data = json::object();
            response.data["categories"] = paginated_data;
            response.data["pagination"] = {
                {"page", pagination.page},
                {"per_page", pagination.per_page},
                {"total_items", pagination.total_items},
                {"total_pages", pagination.total_pages}};
            response.success = true;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error processing categories: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a specific Catppuccin category by its key.
     * @param {const std::string&} key - The unique key of the category.
     * @returns {ApiResponse} Response containing category data or a "not found" error.
     */
    ApiResponse CatppuccinAPI::getCategory(const std::string &key)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(CATEGORIES))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(CATEGORIES);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(CATEGORIES);

            if (data.is_array())
            {
                for (const auto &category : data)
                {
                    if (category.contains("key") && category["key"] == key)
                    {
                        response.data = category;
                        response.success = true;
                        return response;
                    }
                }
            }

            response.error_message = "Category not found: " + key;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error finding category: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a paginated list of Catppuccin showcases.
     * @param {int} page - The page number to retrieve. Defaults to 1.
     * @param {int} per_page - The number of items per page. Defaults to 20.
     * @returns {ApiResponse} Response containing showcase data or an error.
     */
    ApiResponse CatppuccinAPI::getShowcases(int page, int per_page)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(PORTS))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(PORTS);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(PORTS);

            if (data.contains("showcases") && data["showcases"].is_array())
            {
                const json &showcases = data["showcases"];

                PaginationInfo pagination = calculatePagination(showcases.size(), page, per_page);
                json paginated_data = paginateArray(showcases, pagination);

                response.data = json::object();
                response.data["showcases"] = paginated_data;
                response.data["pagination"] = {
                    {"page", pagination.page},
                    {"per_page", pagination.per_page},
                    {"total_items", pagination.total_items},
                    {"total_pages", pagination.total_pages}};
                response.success = true;
            }
            else
            {
                response.error_message = "No showcases found";
            }
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error processing showcases: " + std::string(e.what());
        }

        return response;
    }
    /*
     * @brief Retrieves a paginated list of Catppuccin userstyles.
     */
    ApiResponse CatppuccinAPI::getUserstyles(int page, int per_page)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(USERSTYLES))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(USERSTYLES);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(USERSTYLES);
            json all_userstyles = json::array();

            if (data.contains("userstyles") && data["userstyles"].is_object())
            {
                for (const auto &[key, userstyle] : data["userstyles"].items())
                {
                    json u = userstyle;
                    u["key"] = key;
                    u["is-userstyle"] = true;
                    all_userstyles.push_back(u);
                }
            }

            PaginationInfo pagination = calculatePagination(all_userstyles.size(), page, per_page);
            json paginated_data = paginateArray(all_userstyles, pagination);

            response.data = json::object();
            response.data["userstyles"] = paginated_data;
            response.data["pagination"] = {
                {"page", pagination.page},
                {"per_page", pagination.per_page},
                {"total_items", pagination.total_items},
                {"total_pages", pagination.total_pages}};
            response.success = true;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error processing userstyles: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves a specific Catppuccin userstyle by its identifier.
     */
    ApiResponse CatppuccinAPI::getUserstyle(const std::string &identifier)
    {
        ApiResponse response;

        if (!fetcher_.isCacheValid(USERSTYLES))
        {
            ApiResponse fetch_result = fetcher_.fetchAndCacheData(USERSTYLES);
            if (!fetch_result.success)
                return fetch_result;
        }

        try
        {
            const json &data = fetcher_.getCachedData(USERSTYLES);

            if (data.contains("userstyles") && data["userstyles"].is_object())
            {
                const auto &userstyles = data["userstyles"];
                if (userstyles.contains(identifier))
                {
                    response.data = userstyles[identifier];
                    response.data["key"] = identifier;
                    response.data["is-userstyle"] = true;
                    response.success = true;
                    return response;
                }
            }

            response.error_message = "Userstyle not found: " + identifier;
        }
        catch (const std::exception &e)
        {
            response.error_message = "Error finding userstyle: " + std::string(e.what());
        }

        return response;
    }

    /*
     * @brief Retrieves the Catppuccin color palette.
     * @returns {ApiResponse} Response containing the color palette data.
     */
    ApiResponse CatppuccinAPI::getPalette()
    {
        ApiResponse response;
        response.data = json::object();
        response.data["latte"] = {
            {"rosewater", "#dc8a78"}, {"flamingo", "#dd7878"}, {"pink", "#ea76cb"}, {"mauve", "#8839ef"}, {"red", "#d20f39"}, {"maroon", "#e64553"}, {"peach", "#fe640b"}, {"yellow", "#df8e1d"}, {"green", "#40a02b"}, {"teal", "#179299"}, {"sky", "#04a5e5"}, {"sapphire", "#209fb5"}, {"blue", "#1e66f5"}, {"lavender", "#7287fd"}, {"text", "#4c4f69"}, {"subtext1", "#5c5f77"}, {"subtext0", "#6c6f85"}, {"overlay2", "#7c7f93"}, {"overlay1", "#8c8fa1"}, {"overlay0", "#9ca0b0"}, {"surface2", "#acb0be"}, {"surface1", "#bcc0cc"}, {"surface0", "#ccd0da"}, {"base", "#eff1f5"}, {"mantle", "#e6e9ef"}, {"crust", "#dce0e8"}};
        response.data["frappe"] = {
            {"rosewater", "#f2d5cf"}, {"flamingo", "#eebebe"}, {"pink", "#f4b8e4"}, {"mauve", "#ca9ee6"}, {"red", "#e78284"}, {"maroon", "#ea999c"}, {"peach", "#ef9f76"}, {"yellow", "#e5c890"}, {"green", "#a6d189"}, {"teal", "#81c8be"}, {"sky", "#99d1db"}, {"sapphire", "#85c1dc"}, {"blue", "#8caaee"}, {"lavender", "#babbf1"}, {"text", "#c6d0f5"}, {"subtext1", "#b5bfe2"}, {"subtext0", "#a5adce"}, {"overlay2", "#949cbb"}, {"overlay1", "#838ba7"}, {"overlay0", "#737994"}, {"surface2", "#626880"}, {"surface1", "#51576d"}, {"surface0", "#414559"}, {"base", "#303446"}, {"mantle", "#292c3c"}, {"crust", "#232634"}};
        response.data["macchiato"] = {
            {"rosewater", "#f4dbd6"}, {"flamingo", "#f0c6c6"}, {"pink", "#f5bde6"}, {"mauve", "#c6a0f6"}, {"red", "#ed8796"}, {"maroon", "#ee99a0"}, {"peach", "#f5a97f"}, {"yellow", "#eed49f"}, {"green", "#a6da95"}, {"teal", "#8bd5ca"}, {"sky", "#91d7e3"}, {"sapphire", "#7dc4e4"}, {"blue", "#8aadf4"}, {"lavender", "#b7bdf8"}, {"text", "#cad3f5"}, {"subtext1", "#b8c0e0"}, {"subtext0", "#a5adcb"}, {"overlay2", "#939ab7"}, {"overlay1", "#8087a2"}, {"overlay0", "#6e738d"}, {"surface2", "#5b6078"}, {"surface1", "#494d64"}, {"surface0", "#363a4f"}, {"base", "#24273a"}, {"mantle", "#1e2030"}, {"crust", "#181926"}};
        response.data["mocha"] = {
            {"rosewater", "#f5e0dc"}, {"flamingo", "#f2cdcd"}, {"pink", "#f5c2e7"}, {"mauve", "#cba6f7"}, {"red", "#f38ba8"}, {"maroon", "#eba0ac"}, {"peach", "#fab387"}, {"yellow", "#f9e2af"}, {"green", "#a6e3a1"}, {"teal", "#94e2d5"}, {"sky", "#89dceb"}, {"sapphire", "#74c7ec"}, {"blue", "#89b4fa"}, {"lavender", "#b4befe"}, {"text", "#cdd6f4"}, {"subtext1", "#bac2de"}, {"subtext0", "#a6adc8"}, {"overlay2", "#9399b2"}, {"overlay1", "#7f849c"}, {"overlay0", "#6c7086"}, {"surface2", "#585b70"}, {"surface1", "#45475a"}, {"surface0", "#313244"}, {"base", "#1e1e2e"}, {"mantle", "#181825"}, {"crust", "#11111b"}};
        response.success = true;

        return response;
    }

    /*
     * @brief Get server metrics and statistics.
     * @returns {ApiResponse} Response containing metrics data such as request counts and error rates.
     */
    ApiResponse CatppuccinAPI::getMetrics()
    {
        ApiResponse response;
        auto now = std::chrono::steady_clock::now();
        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();

        response.data = json::object();
        response.data["uptime_seconds"] = uptime;
        response.data["total_requests"] = request_count_.load();
        response.data["total_errors"] = error_count_.load();
        response.data["requests_per_second"] = uptime > 0 ? (double)request_count_.load() / uptime : 0;
        response.data["error_rate"] = request_count_.load() > 0 ? (double)error_count_.load() / request_count_.load() : 0;

        response.success = true;

        return response;
    }

    /*
     * @brief Clears all cached data.
     */
    void CatppuccinAPI::clearCache()
    {
        fetcher_.clearCache();
    }

    /*
     * @brief Refreshes all cached data by clearing and re-fetching it.
     */
    void CatppuccinAPI::refreshCache()
    {
        fetcher_.refreshCache();
    }
}
