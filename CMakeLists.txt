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
add_subdirectory(third_party/fmt)
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
