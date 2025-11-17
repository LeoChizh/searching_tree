cmake_minimum_required(VERSION 3.15)
project(SearchingTree VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

# Include directories
include_directories(include)

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Commands.cpp
)

# Target include directories (modern approach)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Optional: Add tests if test directory exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
    enable_testing()
    add_subdirectory(test)
endif()