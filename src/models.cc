/*
 * @file models.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file implements the utility functions defined in `models.hpp`.
 * These functions include conversions between DataType enum and string,
 * and serialization of various data structures into JSON objects.
 */
#include "models.hpp"

namespace Catppuccin
{
/*
 * @brief Converts a DataType enum value to its corresponding string representation.
 * @param {DataType} type - The DataType enum value to convert.
 * @returns {std::string} The string representation. Returns "unknown" for unrecognized types.
 */
std::string dataTypeToString(DataType type)
{
    switch (type)
    {
        case PORTS:
            return "ports";
        case USERSTYLES:
            return "userstyles";
        case CATEGORIES:
            return "categories";
        default:
            return "unknown";
    }
}

/*
 * @brief Converts a string representation to its corresponding DataType enum value.
 * @param {const std::string&} str - The string to convert.
 * @returns {DataType} The DataType enum value. Defaults to PORTS for unrecognized strings.
 */
DataType stringToDataType(const std::string &str)
{
    if (str == "ports")
        return PORTS;
    if (str == "userstyles")
        return USERSTYLES;
    if (str == "categories")
        return CATEGORIES;
    return PORTS;  // default
}

/*
 * @brief Converts a Port struct to a JSON object.
 * @param {const Port&} port - The Port struct instance to be converted.
 * @returns {json} A `nlohmann::json` object representing the Port.
 */
json portToJson(const Port &port)
{
    json j;
    j["name"] = port.name;
    j["key"] = port.key;
    j["categories"] = port.categories;
    j["platform"] = port.platform;
    j["color"] = port.color;
    j["is_userstyle"] = port.is_userstyle;
    j["repository"]["name"] = port.repository.name;
    j["repository"]["url"] = port.repository.url;
    j["repository"]["current_maintainers"] = port.repository.current_maintainers;
    j["repository"]["past_maintainers"] = port.repository.past_maintainers;

    if (port.icon.has_value())
    {
        j["icon"] = port.icon.value();
    }
    if (port.upstreamed.has_value())
    {
        j["upstreamed"] = port.upstreamed.value();
    }
    if (port.links.has_value())
    {
        json links_array = json::array();
        for (const auto &link_item : port.links.value())
        {
            json link_json;
            link_json["name"] = link_item.name;
            link_json["url"] = link_item.url;
            if (link_item.color.has_value())
            {
                link_json["color"] = link_item.color.value();
            }
            if (link_item.icon.has_value())
            {
                link_json["icon"] = link_item.icon.value();
            }
            links_array.push_back(link_json);
        }
        j["links"] = links_array;
    }

    return j;
}

/*
 * @brief Converts a Collaborator struct to a JSON object.
 * @param {const Collaborator&} collaborator - The Collaborator struct instance.
 * @returns {json} A `nlohmann::json` object representing the Collaborator.
 */
json collaboratorToJson(const Collaborator &collaborator)
{
    json j;
    j["username"] = collaborator.username;
    j["url"] = collaborator.url;
    return j;
}

/*
 * @brief Converts a Category struct to a JSON object.
 * @param {const Category&} category - The Category struct instance.
 * @returns {json} A `nlohmann::json` object representing the Category.
 */
json categoryToJson(const Category &category)
{
    json j;
    j["key"] = category.key;
    j["name"] = category.name;
    j["description"] = category.description;
    j["emoji"] = category.emoji;
    return j;
}

/*
 * @brief Converts a Showcase struct to a JSON object.
 * @param {const Showcase&} showcase - The Showcase struct instance.
 * @returns {json} A `nlohmann::json` object representing the Showcase.
 */
json showcaseToJson(const Showcase &showcase)
{
    json j;
    j["title"] = showcase.title;
    j["description"] = showcase.description;
    j["link"] = showcase.link;
    return j;
}
}  // namespace Catppuccin
