#include "Dropoff.hpp"

#include "catch.hpp"

using namespace hlt;

SCENARIO("Dropoff is initialized correctly", "[dropoff]") {
    GIVEN("A dropoff") {
        Dropoff dropoff(Dropoff::id_type(0), {0, 0});
        REQUIRE(dropoff.id == Dropoff::id_type(0));
        REQUIRE(dropoff.location == Location{0, 0});
        WHEN("converted to bot serial format") {
            std::stringstream stream;
            stream << dropoff;
            THEN("output correct value") {
                REQUIRE(stream.str() == "0 0 0");
            }
        }
    }
}
