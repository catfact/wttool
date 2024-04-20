add_executable(test_osc_sweep test_osc_sweep.cpp)

if (UNIX)
    if (APPLE)
        target_include_directories(test_osc_sweep PRIVATE /opt/local/include)
        target_link_directories(test_osc_sweep PRIVATE /opt/local/lib)
    else ()
        target_link_libraries(test_osc_sweep sndfile)
    endif ()
    target_link_libraries(test_osc_sweep sndfile)
else ()
    # no windows yet
endif ()

