#include <doctest/doctest.h>

#include "pipeline/Components.hpp"

TEST_SUITE_BEGIN("pipeline builder");

TEST_CASE("base pipline") {
  auto pipeline = late::pipeline::make_pipeline<int>();
  auto x        = GENERATE(1, 2, 3);
  std::optional<int> y;
  pipeline << x;
  pipeline >> y;
  CHECK(x == *y);
}

TEST_CASE("output but no input") {
  auto pipeline = late::pipeline::make_pipeline<int>();
  std::optional<int> y;
  pipeline >> y;
  CHECK_FALSE(y);
}

TEST_CASE("one input then double output") {
  auto pipeline = late::pipeline::make_pipeline<int>();
  auto x        = GENERATE(1, 2, 3);
  std::optional<int> y;
  pipeline << x;
  pipeline >> y;
  CHECK(x == *y);
  pipeline >> y;
  CHECK_FALSE(y);
}

TEST_CASE("chained pipeline") {
  struct MockPipeline : public late::pipeline::IStage<int, double> {
    auto process(int&& input) -> double override {
      return static_cast<double>(input) / 2.0;
    };
  };
  auto pipeline = late::pipeline::make_pipeline<int>().attach<MockPipeline>();
  auto x        = GENERATE(1, 2, 3);
  std::optional<double> y;
  pipeline << x;
  pipeline >> y;
  CHECK(*y == x / 2.0);
}

TEST_SUITE_END();
