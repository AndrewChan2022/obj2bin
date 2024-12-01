cmake_minimum_required(VERSION 3.18)

# project
project(obj2bin)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


########################
# external
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(third_party/tinyobjloader)
add_subdirectory(third_party/fmt EXCLUDE_FROM_ALL)  # not install fmt
add_subdirectory(third_party/cxxopts)

set_target_properties(tinyobjloader PROPERTIES FOLDER "ThirdParty")
set_target_properties(fmt PROPERTIES FOLDER "ThirdParty")
set_target_properties(cxxopts PROPERTIES FOLDER "ThirdParty")


########################
# exe 

# source
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(obj2bin 
    ${SRC}
)

# inlcude
target_include_directories(obj2bin PRIVATE
    ${SRC}
)

# lib
target_link_libraries(obj2bin PRIVATE 
    tinyobjloader
    cxxopts
    fmt::fmt
)



########################
# exe 


# force not Program Files(x86)
if (MSVC)
    # if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}" CACHE PATH "Default installation directory" FORCE)
    message(STATUS "Default install prefix set to ${CMAKE_INSTALL_PREFIX}")
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Install the executable to C:/Program Files/obj2bin/bin/obj2bin.exe
install(TARGETS obj2bin
    DESTINATION bin
)

# Manually install only the necessary files, excluding fmt if it's private
# install(FILES ${CMAKE_BINARY_DIR}/$<CONFIG>/obj2bin.exe DESTINATION "C:/Program Files/obj2bin/bin")

########################
# add to $env:PATH


install(SCRIPT ${CMAKE_CURRENT_LIST_DIR}/post_install.cmake)
# install(CODE "
#     include(${CMAKE_CURRENT_LIST_DIR}/post_install.cmake)
# ")


# # Step 1: Write the batch script to a temporary directory (during build)
# set(PATH_UPDATE_SCRIPT "${CMAKE_BINARY_DIR}/update_path.bat")

# file(WRITE ${PATH_UPDATE_SCRIPT} "set TESTPATH \"$ENV{TESTPATH};C:/Program Files/obj2bin/bin\"\n")

# # # Step 2: Install the batch script to the target directory
# install(FILES ${PATH_UPDATE_SCRIPT} DESTINATION "C:/Program Files/obj2bin")

# # Step 3: Run the batch script after installation (during post-install)
# # CMake's `execute_process()` will run the script only after installation is done
# # add_custom_command(TARGET obj2bin POST_INSTALL
# #     COMMAND ${CMAKE_COMMAND} -E echo "Running post-install script to update PATH..."
# #     COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Running update_path.bat to modify PATH"
# #     COMMAND ${CMAKE_COMMAND} -E copy ${PATH_UPDATE_SCRIPT} "C:/Program Files/obj2bin/"
# #     COMMAND ${CMAKE_COMMAND} -E remove ${PATH_UPDATE_SCRIPT}
# # )

# file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/update_path.cmd"
#               CONTENT "set PATH=${LibFoo_RUNTIME_LIBRARY_DIRS};%PATH%
#                        %1 ${CMAKE_CURRENT_BINARY_DIR}/Table.h")
# add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Table.h"
#                    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/RunTableGenerator.cmd" "$<TARGET_FILE:TableGenerator>"
#                    DEPENDS TableGenerator)

# message(STATUS "COMMAND:${CMAKE_COMMAND}")

# # Run cmake -E echo and capture output
# execute_process(
#     COMMAND ${CMAKE_COMMAND} -E echo "********** Running post-install script to update PATH..."
#     OUTPUT_VARIABLE output
#     ERROR_VARIABLE errors
#     RESULT_VARIABLE result
# )

# # Print the result, output, and errors
# MESSAGE(STATUS "Result: ${result}")
# MESSAGE(STATUS "Output: ${output}")
# MESSAGE(STATUS "Errors: ${errors}")

# # Check for errors and print a message
# if(NOT result EQUAL 0)
#     MESSAGE(FATAL_ERROR "Command failed with result: ${result}")
# endif()

# message(STATUS "************* ${PATH_UPDATE_SCRIPT}")

# install(CODE "
#     MESSAGE(\"************ Sample install message.\")
#     MESSAGE(\"************ Sample install message.\")

#     # Run cmake -E echo and capture output
#     execute_process(
#         COMMAND ${CMAKE_COMMAND} -E echo \"********** Running post-install script to update PATH...\"
#         COMMAND ${CMAKE_COMMAND} -E ${PATH_UPDATE_SCRIPT}
#         OUTPUT_VARIABLE outputx
#         ERROR_VARIABLE errorsx
#         RESULT_VARIABLE resultx
#     )

#     # set(SCRIPT_PATH \${CMAKE_BINARY_DIR}/update_path.bat)
#     # execute_process(
#     #     COMMAND \${SCRIPT_PATH}
#     #     RESULT_VARIABLE result
#     #     OUTPUT_VARIABLE output
#     #     ERROR_VARIABLE errors
#     # )

#     # windows install(CODE ...) is executed with limited environment, so cannot capture outout

#     # Print the result, output, and errors
#     # MESSAGE(STATUS \"***** Result: ${resultx}\")
#     # MESSAGE(STATUS \"***** Output: ${outputx}\")
#     # MESSAGE(STATUS \"***** Errors: ${errorsx}\")

#     # Check for errors and print a message
#     # if(NOT resultx EQUAL 0)
#     #     MESSAGE(FATAL_ERROR \"Command failed with result: ${resultx}\")
#     # endif()
# ")

# install(CODE "
#     execute_process(COMMAND ${CMAKE_COMMAND} -E echo \"**********Running post-install script to update PATH...\")
#     execute_process(COMMAND ${CMAKE_COMMAND} -E echo \"**********Running post-install script to update PATH...\")
# ")


# execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue \"Running update_path.bat to modify PATH\")
#     execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PATH_UPDATE_SCRIPT} \"C:/Program Files/obj2bin/\")
#     execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${PATH_UPDATE_SCRIPT}