#include "../include/catch2.hpp"
#include "../src/models.hpp"

using namespace Catppuccin;

TEST_CASE("PaginationInfo calculates correctly", "[utils]")
{
    SECTION("Default constructor sets correct values")
    {
        PaginationInfo pagination;
        REQUIRE(pagination.page == 1);
        REQUIRE(pagination.per_page == 20);
        REQUIRE(pagination.total_items == 0);
        REQUIRE(pagination.total_pages == 0);
    }

    SECTION("Custom constructor works")
    {
        PaginationInfo pagination(3, 10);
        REQUIRE(pagination.page == 3);
        REQUIRE(pagination.per_page == 10);
        REQUIRE(pagination.total_items == 0);
        REQUIRE(pagination.total_pages == 0);
    }
}

TEST_CASE("Data structures work correctly", "[utils]")
{
    SECTION("Repository constructor")
    {
        Repository repo("test", "https://github.com/test/test");
        REQUIRE(repo.name == "test");
        REQUIRE(repo.url == "https://github.com/test/test");
        REQUIRE(repo.current_maintainers.empty());
        REQUIRE(repo.past_maintainers.empty());
    }

    SECTION("Link constructor")
    {
        Link link("Homepage", "https://example.com");
        REQUIRE(link.name == "Homepage");
        REQUIRE(link.url == "https://example.com");
        REQUIRE(link.color.has_value() == false);
        REQUIRE(link.icon.has_value() == false);
    }
}
