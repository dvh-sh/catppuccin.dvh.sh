/*
 * @file models.hpp
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file defines the data structures (structs, enums) and utility functions
 * used throughout the Catppuccin API project. It includes definitions for API
 * responses, pagination, data locations, and various Catppuccin-specific entities
 * like Ports, Collaborators, Categories, and Showcases.
 */
#ifndef CATPPUCCIN_MODELS_HPP
#define CATPPUCCIN_MODELS_HPP

#include "../include/json.hpp"
#include <string>
#include <vector>

namespace Catppuccin
{
    using json = nlohmann::json;

    /*
     * @class optional
     * @brief A simple optional-like class for C++11 compatibility.
     *
     * @description
     * Provides a basic implementation of an optional type, allowing a value
     * to be either present or absent.
     *
     * @template {T} The type of the value to be held.
     */
    template <typename T>
    class optional
    {
    private:
        bool has_value_;
        T value_;

    public:
        optional() : has_value_(false) {}
        optional(const T &value) : has_value_(true), value_(value) {}

        // Add assignment operators
        optional &operator=(const T &value)
        {
            has_value_ = true;
            value_ = value;
            return *this;
        }

        optional &operator=(const optional &other)
        {
            has_value_ = other.has_value_;
            if (has_value_)
            {
                value_ = other.value_;
            }
            return *this;
        }

        bool has_value() const { return has_value_; }
        const T &value() const { return value_; }
        T &value() { return value_; }

        const T &operator*() const { return value_; }
        T &operator*() { return value_; }
    };

    /*
     * @enum DataType
     * @brief Enumerates the different types of data managed by the API.
     */
    enum DataType
    {
        PORTS,      ///< Data related to Catppuccin "ports" (applications/themes).
        USERSTYLES, ///< Data related to Catppuccin userstyles.
        CATEGORIES  ///< Data related to Catppuccin categories.
    };

    /*
     * @struct ApiResponse
     * @brief Structure for standard API responses.
     *
     * @description
     * Encapsulates the outcome of an API operation, including success status,
     * an optional error message, and the JSON data payload.
     */
    struct ApiResponse
    {
        bool success;              ///< True if the API call was successful.
        std::string error_message; ///< Error message if the call failed.
        json data;                 ///< The JSON data payload.

        ApiResponse(bool success = false) : success(success) {}
    };

    /*
     * @struct PaginationInfo
     * @brief Holds information for API response pagination.
     */
    struct PaginationInfo
    {
        int page;        ///< The current page number.
        int per_page;    ///< The number of items per page.
        int total_items; ///< The total number of items available.
        int total_pages; ///< The total number of pages.

        PaginationInfo(int page = 1, int per_page = 20)
            : page(page), per_page(per_page), total_items(0), total_pages(0) {}
    };

    /*
     * @struct DataLocation
     * @brief Defines the location and schema paths for external data sources.
     */
    struct DataLocation
    {
        std::string repository;    ///< GitHub repository slug (e.g., "catppuccin/catppuccin").
        std::string json_path;     ///< Path to the JSON data file.
        std::string schema_path;   ///< Path to the JSON schema file.
        std::string fallback_hash; ///< Git commit hash for fallback data retrieval.

        DataLocation() {}
        DataLocation(const std::string &repo, const std::string &json_path,
                     const std::string &schema_path, const std::string &fallback_hash)
            : repository(repo), json_path(json_path), schema_path(schema_path),
              fallback_hash(fallback_hash) {}
    };

    /*
     * @struct Link
     * @brief Represents a generic link with optional styling information.
     */
    struct Link
    {
        std::string name;            ///< Display name of the link.
        std::string url;             ///< The URL the link points to.
        optional<std::string> color; ///< Optional color associated with the link.
        optional<std::string> icon;  ///< Optional icon identifier for the link.

        Link(const std::string &name, const std::string &url)
            : name(name), url(url) {}
    };

    /*
     * @struct Repository
     * @brief Details about a GitHub repository.
     */
    struct Repository
    {
        std::string name;                             ///< Name of the repository.
        std::string url;                              ///< URL of the repository.
        std::vector<std::string> current_maintainers; ///< List of current maintainer GitHub usernames.
        std::vector<std::string> past_maintainers;    ///< List of past maintainer GitHub usernames.

        Repository(const std::string &name, const std::string &url)
            : name(name), url(url) {}
    };

    /*
     * @struct Port
     * @brief Represents a Catppuccin "port" (application or theme).
     */
    struct Port
    {
        std::string name;                    ///< Display name of the port.
        std::vector<std::string> categories; ///< Categories the port belongs to.
        std::vector<std::string> platform;   ///< Platforms the port supports.
        std::string color;                   ///< Dominant color associated with the port.
        std::string key;                     ///< Unique identifier key for the port.
        Repository repository;               ///< Repository details for the port.
        optional<std::string> icon;          ///< Optional icon identifier.
        optional<bool> upstreamed;           ///< Optional flag indicating if it's upstreamed.
        optional<std::vector<Link>> links;   ///< Optional list of associated links.
        bool is_userstyle;                   ///< True if this port is a userstyle.

        Port(const std::string &name, const std::string &key, const Repository &repo)
            : name(name), key(key), repository(repo), is_userstyle(false) {}
    };

    /*
     * @struct Collaborator
     * @brief Represents a Catppuccin project collaborator.
     */
    struct Collaborator
    {
        std::string username; ///< GitHub username of the collaborator.
        std::string url;      ///< URL to the collaborator's GitHub profile.

        Collaborator(const std::string &username, const std::string &url)
            : username(username), url(url) {}
    };

    /*
     * @struct Category
     * @brief Represents a category for Catppuccin ports.
     */
    struct Category
    {
        std::string key;         ///< Unique identifier key for the category.
        std::string name;        ///< Display name of the category.
        std::string description; ///< A brief description of the category.
        std::string emoji;       ///< An emoji representing the category.

        Category(const std::string &key, const std::string &name,
                 const std::string &description, const std::string &emoji)
            : key(key), name(name), description(description), emoji(emoji) {}
    };

    /*
     * @struct Showcase
     * @brief Represents a project showcase.
     */
    struct Showcase
    {
        std::string title;       ///< Title of the showcase.
        std::string description; ///< Description of the showcase.
        std::string link;        ///< URL to the showcased project or image.

        Showcase(const std::string &title, const std::string &description,
                 const std::string &link)
            : title(title), description(description), link(link) {}
    };

    /*
     * @brief Converts a DataType enum value to its string representation.
     * @param {DataType} type - The DataType enum value.
     * @returns {std::string} The string representation.
     */
    std::string dataTypeToString(DataType type);

    /*
     * @brief Converts a string representation to its DataType enum value.
     * @param {const std::string&} str - The string to convert.
     * @returns {DataType} The DataType enum value.
     */
    DataType stringToDataType(const std::string &str);

    /*
     * @brief Converts a Port struct to a JSON object.
     * @param {const Port&} port - The Port struct.
     * @returns {json} A JSON object representing the Port.
     */
    json portToJson(const Port &port);

    /*
     * @brief Converts a Collaborator struct to a JSON object.
     * @param {const Collaborator&} collaborator - The Collaborator struct.
     * @returns {json} A JSON object representing the Collaborator.
     */
    json collaboratorToJson(const Collaborator &collaborator);

    /*
     * @brief Converts a Category struct to a JSON object.
     * @param {const Category&} category - The Category struct.
     * @returns {json} A JSON object representing the Category.
     */
    json categoryToJson(const Category &category);

    /*
     * @brief Converts a Showcase struct to a JSON object.
     * @param {const Showcase&} showcase - The Showcase struct.
     * @returns {json} A JSON object representing the Showcase.
     */
    json showcaseToJson(const Showcase &showcase);
}

#endif // CATPPUCCIN_MODELS_HPP
