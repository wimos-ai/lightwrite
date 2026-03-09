cmake_minimum_required(VERSION 3.16)
project(Nitewrite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)  # Enables testing

# -------------------------
# Dependencies
# -------------------------

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Fetch GoogleTest automatically
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: prevent overriding parent project's compiler settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# -------------------------
# Source files
# -------------------------

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
)

# -------------------------
# Core library (IMPORTANT)
# -------------------------

add_library(MyProjectLib ${SOURCES})

target_include_directories(MyProjectLib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MyProjectLib PUBLIC
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
)

target_compile_definitions(MyProjectLib PUBLIC
    DATA_DIR="/home/willm/Desktop/lightwrite/data"
)

# -------------------------
# Main executable
# -------------------------
# Comment the below out for GDB
# set(PROJECT_ROOT ${CMAKE_SOURCE_DIR}/)
# cmake_path(NORMAL_PATH PROJECT_ROOT OUTPUT_VARIABLE PROJECT_ROOT_NORMALIZED)
# add_compile_options(-ffile-prefix-map=${PROJECT_ROOT_NORMALIZED}=)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/main.cpp)  # adjust if needed
target_link_libraries(${PROJECT_NAME} PRIVATE MyProjectLib)

# -------------------------
# Tests
# -------------------------

if(BUILD_TESTING AND TEST_FILES)
    add_executable(${PROJECT_NAME}_tests ${TEST_FILES})

    target_link_libraries(${PROJECT_NAME}_tests PRIVATE
        MyProjectLib
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
endif()