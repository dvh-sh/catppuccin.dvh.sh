#include "../include/catch2.hpp"
#include "../src/models.hpp"

using namespace Catppuccin;

TEST_CASE("DataType conversion functions work correctly", "[models]")
{
    SECTION("dataTypeToString converts correctly")
    {
        REQUIRE(dataTypeToString(PORTS) == "ports");
        REQUIRE(dataTypeToString(USERSTYLES) == "userstyles");
        REQUIRE(dataTypeToString(CATEGORIES) == "categories");
    }

    SECTION("stringToDataType converts correctly")
    {
        REQUIRE(stringToDataType("ports") == PORTS);
        REQUIRE(stringToDataType("userstyles") == USERSTYLES);
        REQUIRE(stringToDataType("categories") == CATEGORIES);
        REQUIRE(stringToDataType("invalid") == PORTS);  // default fallback
    }
}

TEST_CASE("ApiResponse works correctly", "[models]")
{
    SECTION("Default constructor sets success to false")
    {
        ApiResponse response;
        REQUIRE(response.success == false);
        REQUIRE(response.error_message.empty());
    }

    SECTION("Constructor with success parameter works")
    {
        ApiResponse response(true);
        REQUIRE(response.success == true);
    }
}

TEST_CASE("JSON serialization works", "[models]")
{
    SECTION("Port to JSON conversion")
    {
        Repository repo("test-repo", "https://github.com/test/repo");
        repo.current_maintainers = {"user1", "user2"};
        repo.past_maintainers = {"user3"};

        Port port("Test Port", "test-port", repo);
        port.categories = {"terminal", "editor"};
        port.platform = {"linux", "macos"};
        port.color = "blue";
        port.icon = "test-icon";

        json result = portToJson(port);

        REQUIRE(result["name"] == "Test Port");
        REQUIRE(result["key"] == "test-port");
        REQUIRE(result["color"] == "blue");
        REQUIRE(result["icon"] == "test-icon");
        REQUIRE(result["categories"].size() == 2);
        REQUIRE(result["platform"].size() == 2);
        REQUIRE(result["repository"]["name"] == "test-repo");
        REQUIRE(result["repository"]["current_maintainers"].size() == 2);
    }

    SECTION("Collaborator to JSON conversion")
    {
        Collaborator collab("testuser", "https://github.com/testuser");
        json result = collaboratorToJson(collab);

        REQUIRE(result["username"] == "testuser");
        REQUIRE(result["url"] == "https://github.com/testuser");
    }

    SECTION("Category to JSON conversion")
    {
        Category category("terminal", "Terminal", "Terminal applications", "💻");
        json result = categoryToJson(category);

        REQUIRE(result["key"] == "terminal");
        REQUIRE(result["name"] == "Terminal");
        REQUIRE(result["description"] == "Terminal applications");
        REQUIRE(result["emoji"] == "💻");
    }
}

TEST_CASE("Optional class works correctly", "[models]")
{
    SECTION("Default constructor creates empty optional")
    {
        optional<std::string> opt;
        REQUIRE(opt.has_value() == false);
    }

    SECTION("Value constructor creates filled optional")
    {
        optional<std::string> opt("test");
        REQUIRE(opt.has_value() == true);
        REQUIRE(opt.value() == "test");
        REQUIRE(*opt == "test");
    }

    SECTION("Copy and assignment work")
    {
        optional<int> opt1(42);
        optional<int> opt2 = opt1;
        REQUIRE(opt2.has_value() == true);
        REQUIRE(opt2.value() == 42);
    }
}
