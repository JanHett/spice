# changes To-Do (https://www.youtube.com/watch?v=bsXLMQ6WgIk):

# remove file(GLOB)

# set up target for lib: `add_library`

# set up target for tests: `add_executable`

# set version number

# remove explicit compile flags, and required version...
# set them on targets:
# --> target_compile_features(<target> PUBLIC ... INTERFACE ... PRIVATE ...)

# specify PUBLIC/INTERFACE/PRIVATE on target_link_libraries

# remove -Wall etc from *requirements* - only use it for building tests

# add install commands and export library interface (38:00 +)

# add alias spice::spice

# CTEST! ctest -S build.cmake <== this is where CI is run

# remove -Werror

# target properties for static analysis (especially CMAK_CXX_CLANG_TIDY)



include(GoogleTest)

cmake_minimum_required(VERSION 3.10)

project(spice LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)


if(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

############################
# OpenImageIO
############################

find_package(OpenImageIO REQUIRED)

# if(NOT OpenImageIO)
#     # we couldn't find the header files for OpenImageIO or they don't exist
#     message("Unable to find OpenImageIO. Attempting to pull and build.")

#     # we have a submodule setup for oiio, so we clone it
#     execute_process(COMMAND git submodule update --init -- external/oiio
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#     add_subdirectory(external/oiio)
# endif()

############################
# Google Test
############################

set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
find_package(GTest REQUIRED)

# if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/")
#     # we couldn't find the header files for GTest or they don't exist
#     message("Unable to find GTest. Attempting to pull.")

#     # we have a submodule setup for googletest, so we clone it
#     execute_process(COMMAND git submodule update --init -- external/googletest
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# set(INSTALL_GTEST OFF)

# add_subdirectory(external/googletest external/googletest/build EXCLUDE_FROM_ALL)

############################
# Guideline Support Library
############################

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/gsl/include/")
    # we couldn't find the header files for GSL or they don't exist
    message("Unable to find Guideline Support Library. Attempting to pull.")

    # we have a submodule setup for gsl, so we clone it
    execute_process(COMMAND git submodule update --init -- external/gsl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
add_subdirectory(external/gsl)

# Main executable config
# file(GLOB SRC
#     src/**/*.h
#     src/**/*.hpp
#     src/**/*.cpp
# )
# add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC})
# target_link_libraries(${PROJECT_NAME} OpenImageIO ${CMAKE_DL_LIBS})
# target_link_libraries(${PROJECT_NAME} GSL)

# Test executable config
file(GLOB TESTS
    tests/*.cpp
    tests/**/*.cpp
)
add_executable(tests ${TESTS})
target_link_libraries(tests
        GTest::GTest
        GSL
        OpenImageIO ${CMAKE_DL_LIBS}
        )
# target_link_libraries(tests PUBLIC OpenImageIO ${CMAKE_DL_LIBS})
# target_link_libraries(tests PUBLIC GSL)
# target_link_libraries(tests PRIVATE GTest::GTest)
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