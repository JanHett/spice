if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../external/m.css/")
    # we couldn't find m.css
    message("Unable to find m.css. Attempting to pull.")

    # we have a submodule setup for m.css, so we clone it
    execute_process(COMMAND git submodule update --init -- external/m.css
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

add_custom_target(doc COMMAND "../external/m.css/documentation/doxygen.py" "Doxyfile-mcss"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")