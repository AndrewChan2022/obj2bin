# post_install.cmake
execute_process(
    # COMMAND python -c "print('hello from post-install script', end='')"
    COMMAND python ${CMAKE_CURRENT_LIST_DIR}/post_install.py
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

# Display the output
message(STATUS "Python command output: ${output}")
if(errors)
    message(STATUS "Python command errors: ${errors}")
endif()

# Check for errors
if(NOT result EQUAL 0)
    message(STATUS "Post-install Python command failed with code: ${result}")
else()
message(STATUS "Post-install Python command success with code: ${result}")
endif()
