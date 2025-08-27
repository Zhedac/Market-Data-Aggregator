

cmake_minimum_required(VERSION 3.10)
project(MarketDataProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(src)

# Source files
file(GLOB_RECURSE LIB_SOURCES
    src/*.cpp
)

list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main.cpp")

# Library
add_library(MarketDataLib STATIC ${LIB_SOURCES})

# Executable
add_executable(MarketDataProcessor src/main.cpp)

if (MINGW OR CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(MarketDataProcessor PRIVATE MarketDataLib stdc++fs)
else()
    target_link_libraries(MarketDataProcessor PRIVATE MarketDataLib)
endif()

# Add tests
enable_testing()
add_subdirectory(tests)
