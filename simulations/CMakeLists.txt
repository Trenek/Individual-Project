project(Trajectory LANGUAGES CXX)
add_executable(Trajectory
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory.cpp"
)
target_link_libraries(Trajectory Common)

project(Angle LANGUAGES CXX)
add_executable(Angle
    "${CMAKE_CURRENT_SOURCE_DIR}/angle.cpp"
)
target_link_libraries(Angle Common)

project(AngleFinder LANGUAGES CXX)
add_executable(AngleFinder
    "${CMAKE_CURRENT_SOURCE_DIR}/angleFinder.cpp"
)
target_link_libraries(AngleFinder Common)
