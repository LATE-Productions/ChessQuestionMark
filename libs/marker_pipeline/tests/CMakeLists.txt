# Method B: FetchContent (no system install needed)
include(FetchContent)
FetchContent_Declare(doctest
				GIT_REPOSITORY https://github.com/doctest/doctest
				GIT_TAG v2.5.3)
FetchContent_MakeAvailable(doctest)
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")  # for doctest_discover_tests

add_executable(tests
	main.cpp
	pipeline_builder.cpp
)
target_link_libraries(tests PRIVATE MarkerPipeline doctest::doctest)
target_compile_features(tests PRIVATE cxx_std_23)

add_test(NAME unit_tests COMMAND tests --success --force-colors)
doctest_discover_tests(tests)

