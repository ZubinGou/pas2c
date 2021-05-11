cmake_minimum_required(VERSION 3.10)
# set the project name and version
project(p2c VERSION 2.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(spdlog_DIR "include/spdlog/build")
add_subdirectory(include/spdlog)

# set(JSON_BuildTests OFF CACHE INTERNAL "")

# configure_file(TutorialConfig.h.in TutorialConfig.h)

# ---------------------------------------------------------------------------------------
# Example of using pre-compiled library
# ---------------------------------------------------------------------------------------
file(GLOB p2c_srcs "src/*.cpp")
# set(p2c_srcs ${main_srcs})

# list(REMOVE_ITEM p2c_srcs ./src/main.cpp)
# list(REMOVE_ITEM main_srcs src/p2c.cpp)
# this is the file I want to exclude / remove from the list
get_filename_component(full_path_test_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)
message("-- Excluded: ${full_path_test_cpp}")
list(REMOVE_ITEM p2c_srcs "${full_path_test_cpp}")

add_executable(p2c ${p2c_srcs})
# add_executable(main ${main_srcs})

# target_link_libraries(main PRIVATE spdlog::spdlog)
target_link_libraries(p2c PRIVATE spdlog::spdlog)

target_include_directories(p2c PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )