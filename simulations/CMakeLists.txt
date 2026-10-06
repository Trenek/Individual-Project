project(Trajectory2D4 LANGUAGES CXX)
add_executable(Trajectory2D4
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory2d4.cpp"
)
target_link_libraries(Trajectory2D4 Common)

project(Trajectory2D3 LANGUAGES CXX)
add_executable(Trajectory2D3
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory2d3.cpp"
)
target_link_libraries(Trajectory2D3 Common)

project(Trajectory2D2 LANGUAGES CXX)
add_executable(Trajectory2D2
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory2d2.cpp"
)
target_link_libraries(Trajectory2D2 Common)

project(Trajectory3D LANGUAGES CXX)
add_executable(Trajectory3D
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory3d.cpp"
)
target_link_libraries(Trajectory3D Common)

project(MaxAngle LANGUAGES CXX)
add_executable(MaxAngle
    "${CMAKE_CURRENT_SOURCE_DIR}/maxAngle.cpp"
)
target_link_libraries(MaxAngle Common)

project(MinimalDistance LANGUAGES CXX)
add_executable(MinimalDistance
    "${CMAKE_CURRENT_SOURCE_DIR}/minimalDistance.cpp"
)
target_link_libraries(MinimalDistance Common)

project(TimeToMinimal LANGUAGES CXX)
add_executable(TimeToMinimal
    "${CMAKE_CURRENT_SOURCE_DIR}/timeToMinimal.cpp"
)
target_link_libraries(TimeToMinimal Common)

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
