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


################################################################################

cmake_minimum_required(VERSION 3.12)

project("spice"
             DESCRIPTION "An image processing library."
             HOMEPAGE_URL "https://github.com/janhett/spice")

set(PROJECT_VERSION 1.0.0-alpha)

include(GNUInstallDirs)

if(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

#############################
# Dependency setup
#############################

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# OpenImageIO
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

find_package(OpenImageIO REQUIRED)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Google Test
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

# set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
# find_package(GTest REQUIRED)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Guideline Support Library
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/gsl/include/")
    # we couldn't find the header files for GSL or they don't exist
    message("Unable to find Guideline Support Library. Attempting to pull.")

    # we have a submodule setup for gsl, so we clone it
    execute_process(COMMAND git submodule update --init -- external/gsl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
add_subdirectory(external/gsl)

#############################
# Libary target setup
#############################

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# TODO: change src to include
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_link_libraries(spice
    INTERFACE
        GSL
        OpenImageIO)

#############################
# Installation setup
#############################

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# install(EXPORT ${PROJECT_NAME}_Targets
#         FILE ${PROJECT_NAME}Targets.cmake
#         NAMESPACE ${PROJECT_NAME}::
#         DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# TODO have source ".../include/spice"?
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION include)

#############################
# Testing setup
#############################

option(ENABLE_TESTS "Build unit tests" ON)

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()

#############################
# Documentation setup
#############################

option(ENABLE_DOCS "Build documentation" ON)

if(ENABLE_DOCS)
  add_subdirectory(doc)
endif()

################################################################################

# cmake_minimum_required(VERSION 3.10)

# # 
# # Project description and (meta) information
# # 

# # Get git revision
# get_git_head_revision(GIT_REFSPEC GIT_SHA1)
# string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)
# if(NOT GIT_SHA1)
#     set(GIT_REV "0")
# endif()

# # Meta information about the project
# set(META_PROJECT_NAME        "spice")
# set(META_PROJECT_DESCRIPTION "spice image processing library")
# set(META_AUTHOR_ORGANIZATION "Jan Hettenkofer")
# set(META_AUTHOR_DOMAIN       "https://github.com/janhett/spice/")
# set(META_AUTHOR_MAINTAINER   "jan@hettenkofer.net")
# set(META_VERSION_MAJOR       "0")
# set(META_VERSION_MINOR       "0")
# set(META_VERSION_PATCH       "1")
# set(META_VERSION_REVISION    "${GIT_REV}")
# set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
# set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")
# set(META_CMAKE_INIT_SHA      "${GIT_REV}")

# project(${META_PROJECT_NAME} LANGUAGES CXX)

# # Project options
# option(OPTION_BUILD_TESTS     "Build tests."              ON)
# option(OPTION_BUILD_DOCS      "Build documentation."      OFF)
# option(OPTION_BUILD_EXAMPLES  "Build examples."           OFF)
# option(OPTION_ENABLE_COVERAGE "Add coverage information." ON)

# file(GLOB SOURCES
#     src/**/*.h
#     src/**/*.hpp
#     src/**/*.cpp
# )

# add_library(spice ${SOURCES})
# add_library(spice::spice ALIAS spice)

# set_target_properties(
#     spice PROPERTIES
#         SOVERSION ${META_VERSION_MAJOR}
#         VERSION ${META_VERSION}
#     )

# target_compile_features(spice PUBLIC cxx_std_17)
# target_compile_options(spice PRIVATE -Wall -Wextra -pedantic)
# target_link_libraries(spice
#     PUBLIC
#         GSL
#         OpenImageIO ${CMAKE_DL_LIBS}
#     )

# ################################################################################


# include(GoogleTest)

# cmake_minimum_required(VERSION 3.10)

# #
# # Configure CMake environment
# #

# # Register general cmake commands
# include(cmake/Custom.cmake)

# # Set policies
# set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
# set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
# set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
# set_policy(CMP0077 NEW) # ENABLE CMP0077: option() honors normal variables

# # Include cmake modules
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include(GenerateExportHeader)

# set(WriterCompilerDetectionHeaderFound NOTFOUND)
# # This module is only available with CMake >=3.1, so check whether it could be found
# # BUT in CMake 3.1 this module doesn't recognize AppleClang as compiler, so just use it as of CMake 3.2
# if (${CMAKE_VERSION} VERSION_GREATER "3.2")
#     include(WriteCompilerDetectionHeader OPTIONAL RESULT_VARIABLE WriterCompilerDetectionHeaderFound)
# endif()

# # Include custom cmake modules
# include(cmake/Coverage.cmake)
# include(cmake/GenerateTemplateExportHeader.cmake)
# include(cmake/GetGitRevisionDescription.cmake)
# include(cmake/HealthCheck.cmake)

# # 
# # Project description and (meta) information
# # 

# # Get git revision
# get_git_head_revision(GIT_REFSPEC GIT_SHA1)
# string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)
# if(NOT GIT_SHA1)
#     set(GIT_REV "0")
# endif()

# # Meta information about the project
# set(META_PROJECT_NAME        "spice")
# set(META_PROJECT_DESCRIPTION "spice image processing library")
# set(META_AUTHOR_ORGANIZATION "Jan Hettenkofer")
# set(META_AUTHOR_DOMAIN       "https://github.com/janhett/spice/")
# set(META_AUTHOR_MAINTAINER   "jan@hettenkofer.net")
# set(META_VERSION_MAJOR       "0")
# set(META_VERSION_MINOR       "0")
# set(META_VERSION_PATCH       "1")
# set(META_VERSION_REVISION    "${GIT_REV}")
# set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
# set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")
# set(META_CMAKE_INIT_SHA      "${GIT_REV}")

# project(${META_PROJECT_NAME} LANGUAGES CXX)

# string(MAKE_C_IDENTIFIER ${META_PROJECT_NAME} META_PROJECT_ID)
# string(TOUPPER ${META_PROJECT_ID} META_PROJECT_ID)


# # 
# # Project configuration options
# # 

# # Project options
# option(BUILD_SHARED_LIBS      "Build shared instead of static libraries."              ON)
# option(OPTION_SELF_CONTAINED  "Create a self-contained install with all dependencies." OFF)
# option(OPTION_BUILD_TESTS     "Build tests."                                           ON)
# option(OPTION_BUILD_DOCS      "Build documentation."                                   OFF)
# option(OPTION_BUILD_EXAMPLES  "Build examples."                                        OFF)
# option(OPTION_ENABLE_COVERAGE "Add coverage information."                              OFF)


# # 
# # Declare project
# # 

# # Generate folders for IDE targets (e.g., VisualStudio solutions)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# set(IDE_FOLDER "")

# # Declare project
# project(${META_PROJECT_NAME} C CXX)

# # Set output directories
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# # Create version file
# file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${META_NAME_VERSION}")


# #
# # Project Health Check Setup
# #

# # Add cmake-init template check cmake targets
# add_check_template_target(${META_CMAKE_INIT_SHA})

# # Configure health check tools
# enable_cppcheck(ON)
# enable_clang_tidy(ON)
# enable_coverage(${OPTION_ENABLE_COVERAGE})

# # 
# # Compiler settings and options
# # 

# include(cmake/CompileOptions.cmake)







# ################################################################################



# if(APPLE)
#     # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
#     LINK_DIRECTORIES(/usr/local/lib)
# endif()

# ############################
# # OpenImageIO
# ############################

# find_package(OpenImageIO REQUIRED)

# # if(NOT OpenImageIO)
# #     # we couldn't find the header files for OpenImageIO or they don't exist
# #     message("Unable to find OpenImageIO. Attempting to pull and build.")

# #     # we have a submodule setup for oiio, so we clone it
# #     execute_process(COMMAND git submodule update --init -- external/oiio
# #                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# #     add_subdirectory(external/oiio)
# # endif()

# ############################
# # Google Test
# ############################

# set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
# find_package(GTest REQUIRED)

# # if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/")
# #     # we couldn't find the header files for GTest or they don't exist
# #     message("Unable to find GTest. Attempting to pull.")

# #     # we have a submodule setup for googletest, so we clone it
# #     execute_process(COMMAND git submodule update --init -- external/googletest
# #                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# # endif()

# # Prevent overriding the parent project's compiler/linker settings on Windows
# # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# # set(INSTALL_GTEST OFF)

# # add_subdirectory(external/googletest external/googletest/build EXCLUDE_FROM_ALL)

# ############################
# # Guideline Support Library
# ############################

# if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/gsl/include/")
#     # we couldn't find the header files for GSL or they don't exist
#     message("Unable to find Guideline Support Library. Attempting to pull.")

#     # we have a submodule setup for gsl, so we clone it
#     execute_process(COMMAND git submodule update --init -- external/gsl
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# endif()
# add_subdirectory(external/gsl)

# ############################
# # Static library target
# ############################

# file(GLOB SRC
#     src/**/*.h
#     src/**/*.hpp
#     src/**/*.cpp
# )
# add_library(spice SRC)

# target_compile_features(spice PUBLIC cxx_std_17)
# target_link_libraries(spice
#     PUBLIC
#         GSL
#         OpenImageIO ${CMAKE_DL_LIBS})

# # Main executable config
# # file(GLOB SRC
# #     src/**/*.h
# #     src/**/*.hpp
# #     src/**/*.cpp
# # )
# # add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC})
# # target_link_libraries(${PROJECT_NAME} OpenImageIO ${CMAKE_DL_LIBS})
# # target_link_libraries(${PROJECT_NAME} GSL)

# ############################
# # Test target
# ############################

# file(GLOB TESTS
#     tests/*.cpp
#     tests/**/*.cpp
# )
# add_executable(tests ${TESTS})
# target_compile_features(tests PUBLIC cxx_std_17)
# target_link_libraries(tests
#     PUBLIC
#         GTest::GTest
#         GSL
#         OpenImageIO ${CMAKE_DL_LIBS}
#         )
# # target_link_libraries(tests PUBLIC OpenImageIO ${CMAKE_DL_LIBS})
# # target_link_libraries(tests PUBLIC GSL)
# # target_link_libraries(tests PRIVATE GTest::GTest)
# gtest_discover_tests(tests)

# if(MSVC)
#     add_compile_options(/W4 /WX)
# else(MSVC)
#     # add_compile_options(-Wall -Wextra -pedantic -Werror)
#     add_compile_options(-Wall -Wextra -pedantic)
#     set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -o1 -fno-omit-frame-pointer -fsanitize=address")
#     set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
#     set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -g -o1 -fno-omit-frame-pointer -fsanitize=address")
#     set (CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} -Werror")
# endif(MSVC)