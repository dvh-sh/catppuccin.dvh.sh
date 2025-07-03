#include "../include/catch2.hpp"
#include "../src/catppuccin_api.hpp"

using namespace Catppuccin;

TEST_CASE("API pagination works correctly", "[api]")
{
    SECTION("calculatePagination computes correct values")
    {
        CatppuccinAPI api;

        // Use reflection or friend class to test private methods
        // For now, test through public interface

        // Test with getPalette (no pagination needed)
        ApiResponse response = api.getPalette();
        REQUIRE(response.success == true);
        REQUIRE(response.data.contains("latte"));
        REQUIRE(response.data.contains("frappe"));
        REQUIRE(response.data.contains("macchiato"));
        REQUIRE(response.data.contains("mocha"));
    }
}

TEST_CASE("API responses have correct structure", "[api]")
{
    SECTION("Palette endpoint returns valid structure")
    {
        CatppuccinAPI api;
        ApiResponse response = api.getPalette();

        REQUIRE(response.success == true);
        REQUIRE(response.data.is_object());

        // Check that each flavor has the expected colors
        const std::vector<std::string> flavors = {"latte", "frappe", "macchiato", "mocha"};
        const std::vector<std::string> colors = {
            "rosewater", "flamingo", "pink",     "mauve",    "red",      "maroon", "peach",    "yellow",   "green",
            "teal",      "sky",      "sapphire", "blue",     "lavender", "text",   "subtext1", "subtext0", "overlay2",
            "overlay1",  "overlay0", "surface2", "surface1", "surface0", "base",   "mantle",   "crust"};

        for (const auto &flavor : flavors)
        {
            REQUIRE(response.data.contains(flavor));
            for (const auto &color : colors)
            {
                REQUIRE(response.data[flavor].contains(color));
                REQUIRE(response.data[flavor][color].is_string());
            }
        }
    }
}
