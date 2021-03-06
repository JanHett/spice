# 
# External dependencies
# 

# find_package(${PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../")

# 
# Executable name and options
# 

# Target name
set(target ${PROJECT_NAME}-test)
message(STATUS "Test ${target}")


# 
# Sources
# 

file(GLOB sources
    *.cpp
    **/*.cpp
)

# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${PROJECT_NAME}::${target} ALIAS ${target})


# 
# Project options
# 

# set_target_properties(${target}
#     PROPERTIES
#     ${DEFAULT_PROJECT_OPTIONS}
#     FOLDER "${IDE_FOLDER}"
# )


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    # ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/external
    ${PROJECT_BINARY_DIR}/include
)


# 
# Libraries
#

include(GoogleTest)
set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
find_package(GTest REQUIRED)

target_link_libraries(${target}
    PRIVATE
    # ${DEFAULT_LIBRARIES}
    ${PROJECT_NAME}::spice
    GTest::GTest
)


# 
# Compile definitions
# 

# target_compile_definitions(${target}
#     PRIVATE
#     ${DEFAULT_COMPILE_DEFINITIONS}
# )


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    -Wall
    -Wextra
    -pedantic
    -g
    -o1
    -fno-omit-frame-pointer
    -fsanitize=address
    # TODO: uncomment once Apple's clang starts supporting this flag
    # -fsanitize=leak
)

target_link_options(${target}
    PRIVATE
    -fsanitize=address
    # TODO: uncomment once Apple's clang starts supporting this flag
    # -fsanitize=leak
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    # ${DEFAULT_LINKER_OPTIONS}
    spice
)