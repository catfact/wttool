cmake_minimum_required(VERSION 3.7)
project(wttool)

add_executable(wttool main.cpp)
if(UNIX)
    if(APPLE)
        find_library(sndfile_lib libsndfile.a REQUIRED)
        target_link_libraries(wttool {sndfile_lib})
        include_directories(/usr/local/include)
    else()
        target_link_libraries(wttool sndfile)
    endif()
else()
    # no windows yet
endif()

set(CMAKE_CXX_STANDARD 14)
