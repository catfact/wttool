cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wttool)

add_executable(wttool
        main.cpp
        lib/kissfft/kiss_fft.c
        lib/kissfft/tools/kiss_fftr.c
        )

if(UNIX)
    if(APPLE)
        target_link_libraries(wttool /usr/local/lib/libsndfile.1.dylib)
        include_directories(/usr/local/include)
    else()
        target_link_libraries(wttool sndfile)
    endif()
else()
    # no windows yet
endif()

include_directories(
        lib
        lib/kissfft
        lib/kissfft/tools)
