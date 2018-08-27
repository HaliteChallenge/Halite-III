#include "nlohmann/json.hpp"

#include "catch.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define protected public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "Grid.hpp"

using namespace hlt;

SCENARIO("Grid constructor initializes member variables", "[grid]") {
    GIVEN("An empty grid") {
        Grid<int> grid;
        REQUIRE(grid.width == 0);
        REQUIRE(grid.height == 0);
    }

    GIVEN("A 128x128 grid") {
        Grid<int> grid(128, 128);
        REQUIRE(grid.width == 128);
        REQUIRE(grid.height == 128);
        WHEN("cells are accessed") {
            THEN("the default value is obtained") {
                REQUIRE(grid.at(0, 0) == 0);
                REQUIRE(grid.at({1, 1}) == 0);
                REQUIRE(grid[{2, 2}] == 0);
            }
        }
        WHEN("cells are updated") {
            grid.at(1, 1) = 1;
            grid.at({2, 2}) = 2;
            grid[{3, 3}] = 3;
            THEN("the values are updated") {
                REQUIRE(grid.at(0, 0) == 0);
                REQUIRE(grid.at(1, 1) == 1);
                REQUIRE(grid.at({2, 2}) == 2);
                REQUIRE(grid[{3, 3}] == 3);
            }
        }
    }

    GIVEN("A 128x128 constant grid") {
        const Grid<int> grid(128, 128);
        REQUIRE(grid.width == 128);
        REQUIRE(grid.height == 128);
        WHEN("cells are accessed") {
            THEN("the default value is obtained") {
                REQUIRE(grid.at(0, 0) == 0);
                REQUIRE(grid.at({1, 1}) == 0);
                REQUIRE(grid[{2, 2}] == 0);
            }
        }
    }
}
