cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wttool)

add_executable(wttool
        main.cpp
        lib/kissfft/kiss_fft.c
        lib/kissfft/tools/kiss_fftr.c
        )


add_executable(test_fftr
        test/test_fftr_main.cpp
        lib/kissfft/kiss_fft.c
        lib/kissfft/tools/kiss_fftr.c
        )


if(UNIX)
    if(APPLE)
        # FIXME: couldn't get this to work with less specificity (find_pkg, &c):
        target_link_libraries(wttool /usr/local/lib/libsndfile.1.dylib)
        target_link_libraries(test_fftr /usr/local/lib/libsndfile.1.dylib)
        include_directories(/usr/local/include)
    else()
        target_link_libraries(wttool sndfile)
        target_link_libraries(test_fftr sndfile)
    endif()
else()
    # no windows yet
endif()

include_directories(
        lib
        lib/kissfft
        lib/kissfft/tools)