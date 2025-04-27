cmake_minimum_required(VERSION 3.18)

# project
project(obj2bin)

set(CMAKE_CXX_STANDARD 17)

set(OBJ2BIN_VERSION_MAJOR 2)
set(OBJ2BIN_VERSION_MINOR 1)
set(OBJ2BIN_VERSION_PATCH 0)
set(OBJ2BIN_VERSION_STRING "${OBJ2BIN_VERSION_MAJOR}.${OBJ2BIN_VERSION_MINOR}.${OBJ2BIN_VERSION_PATCH}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
)


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
    ${CMAKE_BINARY_DIR}
)

# lib
target_link_libraries(obj2bin PRIVATE 
    tinyobjloader
    cxxopts
    fmt::fmt
)



########################
# install


# force not Program Files(x86)
if (MSVC)
    # if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}" CACHE PATH "Default installation directory" FORCE)
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/mytools" CACHE PATH "Default installation directory" FORCE)
    message(STATUS "Default install prefix set to ${CMAKE_INSTALL_PREFIX}")
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Install the executable to C:/Program Files/mytools/bin/obj2bin.exe
install(TARGETS obj2bin
    DESTINATION bin
)

# Manually install only the necessary files, excluding fmt if it's private
# install(FILES ${CMAKE_BINARY_DIR}/$<CONFIG>/obj2bin.exe DESTINATION "C:/Program Files/obj2bin/bin")

########################
# post install

# add path to $env:PATH
# if (MSVC)
#     install(SCRIPT ${CMAKE_CURRENT_LIST_DIR}/post_install.cmake)
# endif()

# info add path to $env:PATH
if (MSVC)
    install(CODE "
        message(STATUS \"********** after install, please add PATH ${CMAKE_INSTALL_PREFIX}/bin **********\")
    ")
else()
    # install(CODE "
    #     message(STATUS \"********** after install, please append to ~/.bashrc: export PATH=\\\"\$PATH:${CMAKE_INSTALL_PREFIX}/bin\\\" **********\")
    # ")
endif()
