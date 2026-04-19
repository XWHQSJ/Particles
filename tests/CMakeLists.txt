include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(physics_test
    physics_test.cpp
    ${CMAKE_SOURCE_DIR}/common/particle.cpp
)

target_include_directories(physics_test PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(physics_test PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(physics_test)
