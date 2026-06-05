project(Trajectory LANGUAGES CXX)
add_executable(Trajectory
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory.cpp"
)
target_link_libraries(Trajectory Common)

project(Trajectory3D LANGUAGES CXX)
add_executable(Trajectory3D
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory3d.cpp"
)
target_link_libraries(Trajectory3D Common)

project(Trajectory2D LANGUAGES CXX)
add_executable(Trajectory2D
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory2d.cpp"
)
target_link_libraries(Trajectory2D Common)

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
