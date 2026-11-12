#include <doctest/doctest.h>

#include "pipeline/Components.hpp"

struct MockPrimaryStage : public late::pipeline::IStage<int, double> {
  constexpr static const double divisor = 2.0;
  auto process(int&& input) -> double override {
    return static_cast<double>(input) / divisor;
  };
};

struct MockSecondaryStage : public late::pipeline::IStage<double, int> {
  constexpr static const double multiplier = 2.0;
  auto process(double&& input) -> int override {
    return static_cast<int>(input * multiplier);
  };
};

TEST_SUITE_BEGIN("Pipline Basic Usage");

struct PipelineFixture {
  late::pipeline::Pipeline<int, int> m_pipeline =
      late::pipeline::make_pipeline<int>();
};
// Creating a pipline with no extra stages should produce the same input and
// output

TEST_CASE_FIXTURE(PipelineFixture, "Basic Usage/Pipe then Flush") {
  auto x = GENERATE(1, 2, 3);
  decltype(m_pipeline)::Output y;
  m_pipeline << x;
  m_pipeline >> y;
  CHECK(x == *y);
}

// Trying to extract an output with no input should return a std::nullopt
TEST_CASE_FIXTURE(PipelineFixture, "Basic Usage/Flush without Piping first") {
  decltype(m_pipeline)::Output y;
  m_pipeline >> y;
  CHECK_FALSE(y);
}

TEST_CASE_FIXTURE(PipelineFixture, "Basic Usage/Pipe then Flush twice") {
  auto x = GENERATE(1, 2, 3);
  decltype(m_pipeline)::Output y;
  m_pipeline << x;
  m_pipeline >> y;
  CHECK(x == *y);
  m_pipeline >> y;
  CHECK_FALSE(y);
}

struct PipelineMultiStageFixture {
  late::pipeline::Pipeline<int, double> m_pipeline =
      late::pipeline::make_pipeline<int>().attach<MockPrimaryStage>();
};

TEST_CASE_FIXTURE(
    PipelineMultiStageFixture,
    "Basic Usage/Second stage produces a determenistic output"
) {
  auto x = GENERATE(1, 2, 3);
  decltype(m_pipeline)::Output y;
  m_pipeline << x;
  m_pipeline >> y;
  CHECK(*y == x / MockPrimaryStage::divisor);
}

TEST_SUITE_END();

TEST_SUITE_BEGIN("Pipeline Stage Hooks");
TEST_CASE("Stage Hooks/Basic Hooks") {
  auto x = GENERATE(1, 2, 3);
  auto pipeline =
      late::pipeline::make_pipeline<int>()
          .attach<MockPrimaryStage>()
          .on_input<MockPrimaryStage>([&x](const int& data) {
            CHECK_MESSAGE(data == x, "Input hook should read input data");
          })
          .on_output<MockPrimaryStage>([&x](const double& data) {
            CHECK_MESSAGE(
                data == x / MockPrimaryStage::divisor,
                "Output hook should read the divided input value"
            );
          });

  auto y = x;

  decltype(pipeline)::Output z;
  pipeline << y;
  pipeline >> z;
  CHECK(*z == x / MockPrimaryStage::divisor);
}

TEST_CASE("Stage Hooks/Duplicate Hooks") {
  auto x = GENERATE(1, 2, 3);

  auto pipeline = late::pipeline::make_pipeline<int>()
                      .attach<MockPrimaryStage>()
                      .on_output<MockPrimaryStage>([&x](const double& data) {
                        FAIL("Only the latest hook should be called");
                      })
                      .on_output<MockPrimaryStage>([&x](const double& data) {
                        CHECK_MESSAGE(
                            data == x / MockPrimaryStage::divisor,
                            "Output hook should read the divided input value"
                        );
                      });

  decltype(pipeline)::Output y;
  pipeline << x;
  pipeline >> y;
  CHECK(*y == x / MockPrimaryStage::divisor);
}

TEST_CASE("Stage Hooks/Order of unique Hook attachment is commutative") {
  auto x = GENERATE(1, 2, 3);

  int hookExecution = 0;
  auto firstHook    = [&hookExecution](const int& data) {
    ++hookExecution;
    MESSAGE("First hook executed");
    CHECK_EQ(hookExecution, 1);
  };
  auto secondHook = [&hookExecution](const double& data) {
    ++hookExecution;
    MESSAGE("Second hook executed");
    CHECK_EQ(hookExecution, 2);
  };

  auto thirdHook = [&hookExecution](const double& data) {
    ++hookExecution;
    MESSAGE("Third hook executed");
    CHECK_EQ(hookExecution, 3);
  };
  auto fourthHook = [&hookExecution](const int& data) {
    ++hookExecution;
    MESSAGE("Fourth hook executed");
    CHECK_EQ(hookExecution, 4);
  };
  auto pipeline = late::pipeline::make_pipeline<int>()
                      .attach<MockPrimaryStage>()
                      .attach<MockSecondaryStage>()
                      .on_output<MockSecondaryStage>(fourthHook)
                      .on_input<MockSecondaryStage>(thirdHook)
                      .on_input<MockPrimaryStage>(firstHook)
                      .on_output<MockPrimaryStage>(secondHook);

  decltype(pipeline)::Output y;
  pipeline << x;
  pipeline >> y;
}
TEST_SUITE_END();
