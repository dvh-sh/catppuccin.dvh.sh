/*
 * @file data_fetcher.hpp
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file defines the DataFetcher class, responsible for fetching external
 * JSON data, caching it, and validating against schemas. It manages data locations
 * and provides an interface for refreshing cached data.
 */
#ifndef DATA_FETCHER_HPP
#define DATA_FETCHER_HPP

#include <map>

#include "models.hpp"

namespace Catppuccin
{
/*
 * @class DataFetcher
 * @brief Handles fetching, caching, and validating external data for the API.
 */
class DataFetcher
{
private:
    std::map<DataType, DataLocation> locations_;  ///< Maps DataType to its remote location details.
    std::map<DataType, json> cache_;              ///< Cache for fetched JSON data, keyed by DataType.
    std::map<DataType, bool> valid_;              ///< Indicates if the cache for a DataType is valid.

public:
    /*
     * @brief Constructs a DataFetcher and initializes data locations.
     */
    DataFetcher();

    /*
     * @brief Fetches data for a specific DataType and caches it.
     * @param {DataType} type - The type of data to fetch.
     * @returns {ApiResponse} The response containing fetched data or an error message.
     */
    ApiResponse fetchAndCacheData(DataType type);

    /*
     * @brief Checks if the cached data for a given DataType is valid.
     * @param {DataType} type - The type of data to check.
     * @returns {bool} True if the cache is valid, false otherwise.
     */
    bool isCacheValid(DataType type);

    /*
     * @brief Retrieves the cached data for a given DataType.
     * @param {DataType} type - The type of data to retrieve.
     * @returns {const json&} A reference to the cached JSON data.
     */
    const json &getCachedData(DataType type);

    /*
     * @brief Clears the entire data cache and invalidates all entries.
     */
    void clearCache();

    /*
     * @brief Refreshes the entire data cache by clearing it.
     */
    void refreshCache();

private:
    /*
     * @brief Constructs the URL for fetching data or schema based on DataType.
     * @param {DataType} type - The type of data.
     * @param {bool} fallback - If true, uses the fallback hash for the URL.
     * @param {bool} schema - If true, constructs the URL for the schema path.
     * @returns {std::string} The constructed URL.
     */
    std::string getURL(DataType type, bool fallback = false, bool schema = false);

    /*
     * @brief Fetches JSON data from a given URL.
     * @param {const std::string&} url - The URL to fetch from.
     * @returns {ApiResponse} The response containing fetched JSON data or an error.
     */
    ApiResponse fetchJSON(const std::string &url);

    /*
     * @brief Validates JSON data against a given schema.
     * @param {const json&} data - The JSON data to validate.
     * @param {const json&} schema - The JSON schema to validate against.
     * @returns {bool} True if the data is valid, false otherwise.
     */
    bool validateSchema(const json &data, const json &schema);
};
}  // namespace Catppuccin

#endif
