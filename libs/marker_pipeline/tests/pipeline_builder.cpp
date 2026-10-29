#include <doctest/doctest.h>

#include "pipeline/Components.hpp"

TEST_SUITE_BEGIN("pipeline builder");

TEST_CASE("base pipline") {
  SUBCASE("make pipeline") {
    auto pipeline = late::pipeline::make_pipeline<int>();
    auto x        = GENERATE(1, 2, 3);
    std::optional<int> y;
    pipeline << x;
    pipeline >> y;
    CHECK(x == *y);
  }
}

TEST_SUITE_END();
