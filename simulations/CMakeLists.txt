add_executable(Amplitude
    "${CMAKE_CURRENT_SOURCE_DIR}/amplitude.cpp"
)
target_link_libraries(Amplitude Common)

add_executable(Trajectory
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory.cpp"
)
target_link_libraries(Trajectory Common)

add_executable(CyclicFinder 
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclicFinder.cpp"
)
target_link_libraries(CyclicFinder Common)
