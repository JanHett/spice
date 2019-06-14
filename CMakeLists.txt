include(GoogleTest)

cmake_minimum_required(VERSION 3.10)

project(spice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

#####################
# Catch2
#####################

find_path(CATCH_INCLUDE_DIR ...)

if((NOT CATCH_INCLUDE_DIR) OR (NOT EXISTS ${CATCH_INCLUDE_DIR})
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find Catch2")

    # we have a submodule setup for Catch2, assume it is under external/Catch2
    # now we need to clone this submodule
    execute_process(COMMAND git submodule update --init -- external/Catch2
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # set CATCH_INCLUDE_DIR properly
    set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2/path/to/include
        CACHE PATH "Catch2 include directory")

    # also install it
    install(DIRECTORY ${CATCH_INCLUDE_DIR}/Catch2 DESTINATION ${some_dest})

    # for convenience setup a target
    add_library(Catch2 INTERFACE)
    target_include_directories(Catch2 INTERFACE
                               $<BUILD_INTERFACE:${CATCH_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:${some_dest}>)

    # need to export target as well
    install(TARGETS Catch2 EXPORT my_export_set DESTINATION ${some_dest})
else()
    # see above, setup target as well
endif()

find_package(OpenImageIO REQUIRED)

# Main executable config
file(GLOB SRC
    src/**/*.h
    src/**/*.hpp
    src/**/*.cpp
)
add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC})
target_link_libraries(${PROJECT_NAME} OpenImageIO)

# Test executable config
file(GLOB TESTS
    tests/*.cpp
    tests/**/*.cpp
)
add_executable(tests ${TESTS})
target_link_libraries(tests GTest::GTest)
target_link_libraries(tests OpenImageIO)
gtest_discover_tests(tests)

if(MSVC)
  add_compile_options(/W4 /WX)
else(MSVC)
  # add_compile_options(-Wall -Wextra -pedantic -Werror)
  add_compile_options(-Wall -Wextra -pedantic)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -o1 -fno-omit-frame-pointer -fsanitize=address")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -g -o1 -fno-omit-frame-pointer -fsanitize=address")
  set (CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} -Werror")
endif(MSVC)
