/*
 * @file data_fetcher.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file implements the DataFetcher class, providing functionality
 * to fetch and manage data from external sources, including caching
 * and basic schema validation.
 */
#include "data_fetcher.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

namespace Catppuccin
{
    /*
     * @brief Constructs a DataFetcher and initializes data locations.
     */
    DataFetcher::DataFetcher()
    {
        locations_[PORTS] = DataLocation(
            "catppuccin/catppuccin",
            "resources/ports.porcelain.json",
            "resources/ports.porcelain.schema.json",
            "a1ce9a7c29c6aa323f43caa88f21bf51faa91c3a");

        locations_[USERSTYLES] = DataLocation(
            "catppuccin/userstyles",
            "scripts/userstyles.yml",
            "scripts/userstyles.schema.json",
            "4ee2fffe0492ec2be6d744f770a1cdaa98226d44");

        locations_[CATEGORIES] = DataLocation(
            "catppuccin/catppuccin",
            "resources/categories.yml",
            "resources/categories.schema.json",
            "a1ce9a7c29c6aa323f43caa88f21bf51faa91c3a");

        valid_[PORTS] = false;
        valid_[USERSTYLES] = false;
        valid_[CATEGORIES] = false;
    }

    /*
     * @brief Constructs the URL for fetching data or schema based on DataType.
     * @param {DataType} type - The type of data.
     * @param {bool} fallback - If true, uses the fallback hash for the URL.
     * @param {bool} schema - If true, constructs the URL for the schema path.
     * @returns {std::string} The constructed URL.
     */
    std::string DataFetcher::getURL(DataType type, bool fallback, bool schema)
    {
        if (locations_.find(type) == locations_.end())
        {
            return "";
        }

        const DataLocation &loc = locations_[type];
        std::string version = fallback ? loc.fallback_hash : "main";
        std::string path = schema ? loc.schema_path : loc.json_path;

        return "https://raw.githubusercontent.com/" + loc.repository + "/" + version + "/" + path;
    }

    /*
     * @brief Fetches JSON data from a given URL.
     * @param {const std::string&} url - The URL to fetch from.
     * @returns {ApiResponse} The response containing fetched JSON data or an error.
     */
    ApiResponse DataFetcher::fetchJSON(const std::string &url)
    {
        ApiResponse response;

        std::ostringstream temp_stream;
        temp_stream << "/tmp/catppuccin_" << getpid() << "_" << time(NULL) << ".data";
        std::string tempfile = temp_stream.str();

        std::string cmd = "curl -s -f -o " + tempfile + " '" + url + "'";

        std::cout << "Fetching: " << url << std::endl;

        if (system(cmd.c_str()) == 0)
        {
            std::ifstream file(tempfile);
            if (file.is_open())
            {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();

                if (!content.empty())
                {
                    std::cout << "Successfully fetched " << content.length() << " bytes" << std::endl;

                    try
                    {
                        // Try parsing as JSON first
                        response.data = json::parse(content);
                        response.success = true;
                        std::cout << "Parsed as JSON successfully" << std::endl;
                    }
                    catch (const std::exception &json_error)
                    {
                        std::cout << "JSON parse failed, trying YAML conversion..." << std::endl;

                        // Try yq for YAML to JSON conversion
                        std::string json_tempfile = tempfile + ".json";
                        std::string yq_cmd = "yq eval -o=json '" + tempfile + "' > " + json_tempfile;

                        if (system(yq_cmd.c_str()) == 0)
                        {
                            std::ifstream json_file(json_tempfile);
                            if (json_file.is_open())
                            {
                                std::string json_content((std::istreambuf_iterator<char>(json_file)), std::istreambuf_iterator<char>());
                                json_file.close();
                                remove(json_tempfile.c_str());

                                response.data = json::parse(json_content);
                                response.success = true;
                                std::cout << "YAML converted to JSON using yq" << std::endl;
                            }
                            else
                            {
                                response.error_message = "Failed to read yq converted JSON";
                            }
                        }
                        else
                        {
                            // Fallback: try alternative yq syntax
                            std::string alt_yq_cmd = "yq . '" + tempfile + "' > " + json_tempfile;
                            if (system(alt_yq_cmd.c_str()) == 0)
                            {
                                std::ifstream json_file(json_tempfile);
                                if (json_file.is_open())
                                {
                                    std::string json_content((std::istreambuf_iterator<char>(json_file)), std::istreambuf_iterator<char>());
                                    json_file.close();
                                    remove(json_tempfile.c_str());

                                    response.data = json::parse(json_content);
                                    response.success = true;
                                    std::cout << "YAML converted to JSON using alternative yq" << std::endl;
                                }
                            }
                            else
                            {
                                response.error_message = "yq not available or failed: " + std::string(json_error.what());
                                remove(json_tempfile.c_str());
                            }
                        }
                    }
                }
                else
                {
                    response.error_message = "Empty response from: " + url;
                }
                remove(tempfile.c_str());
            }
            else
            {
                response.error_message = "Can't read temp file: " + tempfile;
                remove(tempfile.c_str());
            }
        }
        else
        {
            response.error_message = "curl failed for: " + url;
            remove(tempfile.c_str());
        }

        return response;
    }

    /*
     * @brief Validates JSON data against a given schema.
     * @param {const json&} data - The JSON data to validate.
     * @param {const json&} schema - The JSON schema to validate against. (Currently only checks if data is object/array)
     * @returns {bool} True if the data is valid, false otherwise.
     */
    bool DataFetcher::validateSchema(const json &data, const json & /*schema*/)
    {
        // Simple validation: check if it's a JSON object or array
        return data.is_object() || data.is_array();
    }

    /*
     * @brief Fetches data for a specific DataType and caches it.
     * @param {DataType} type - The type of data to fetch.
     * @returns {ApiResponse} The response containing fetched data or an error message.
     */
    ApiResponse DataFetcher::fetchAndCacheData(DataType type)
    {
        std::cout << "Fetching data for type: " << dataTypeToString(type) << std::endl;

        std::string main_url = getURL(type, false, false);
        if (main_url.empty())
        {
            ApiResponse error_response;
            error_response.error_message = "No URL configuration for type: " + dataTypeToString(type);
            return error_response;
        }

        ApiResponse response = fetchJSON(main_url);

        if (response.success)
        {
            std::cout << "Successfully cached data for " << dataTypeToString(type) << std::endl;
            cache_[type] = response.data;
            valid_[type] = true;
            return response;
        }

        std::cout << "Main fetch failed, trying fallback..." << std::endl;

        std::string fallback_url = getURL(type, true, false);
        response = fetchJSON(fallback_url);

        if (response.success)
        {
            std::cout << "Fallback successful for " << dataTypeToString(type) << std::endl;
            cache_[type] = response.data;
            valid_[type] = true;
        }
        else
        {
            response.error_message = "Failed to fetch from both main and fallback URLs";
        }

        return response;
    }

    /*
     * @brief Checks if the cached data for a given DataType is valid.
     * @param {DataType} type - The type of data to check.
     * @returns {bool} True if the cache is valid, false otherwise.
     */
    bool DataFetcher::isCacheValid(DataType type)
    {
        return valid_.find(type) != valid_.end() && valid_[type];
    }

    /*
     * @brief Retrieves the cached data for a given DataType.
     * @param {DataType} type - The type of data to retrieve.
     * @returns {const json&} A reference to the cached JSON data.
     */
    const json &DataFetcher::getCachedData(DataType type)
    {
        return cache_[type];
    }

    /*
     * @brief Clears the entire data cache and invalidates all entries.
     */
    void DataFetcher::clearCache()
    {
        cache_.clear();
        valid_[PORTS] = false;
        valid_[USERSTYLES] = false;
        valid_[CATEGORIES] = false;
    }

    /*
     * @brief Refreshes the entire data cache by clearing it.
     */
    void DataFetcher::refreshCache()
    {
        clearCache();
    }
}
