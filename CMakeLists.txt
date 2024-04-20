cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wttool)

add_executable(wttool
        main.cpp
        lib/kissfft/kiss_fft.c
        lib/kissfft/tools/kiss_fftr.c
        )

if (UNIX)
    if (APPLE)
        target_include_directories(wttool PRIVATE /opt/local/include)
        target_link_directories(wttool PRIVATE /opt/local/lib)
    endif ()
    target_link_libraries(wttool sndfile)
else ()
    # no windows yet
endif ()

target_include_directories(wttool PRIVATE
        lib
        lib/kissfft
        lib/kissfft/tools
        )


add_subdirectory(test)
