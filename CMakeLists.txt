include(GoogleTest)

cmake_minimum_required(VERSION 3.15)

project(spice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)


if(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

#####################
# Google Test
#####################

set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
find_package(GTest REQUIRED)

#####################
# OpenImageIO
#####################

find_package(OpenImageIO REQUIRED)

# Main executable config
file(GLOB SRC
    src/**/*.h
    src/**/*.hpp
    src/**/*.cpp
)
add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC})
target_link_libraries(${PROJECT_NAME} OpenImageIO ${CMAKE_DL_LIBS})

# Test executable config
file(GLOB TESTS
    tests/*.cpp
    tests/**/*.cpp
)
add_executable(tests ${TESTS})
target_link_libraries(tests GTest::GTest)
target_link_libraries(tests OpenImageIO ${CMAKE_DL_LIBS})
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