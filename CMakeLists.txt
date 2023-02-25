cmake_minimum_required (VERSION 3.8)

project(panorama)

add_executable (cubemap2panorama "cubemap2panorama.cpp")
add_executable (panoramaviewer "panoramaviewer.cpp")

set(BUILD_SHARED_LIBS FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_AUDIO FALSE)
add_subdirectory(SFML_dir SFML)

include_directories(./src)
include_directories(./src/headers)

target_link_libraries(cubemap2panorama sfml-graphics sfml-system sfml-main)
target_compile_features(cubemap2panorama PUBLIC cxx_std_20)

target_link_libraries(panoramaviewer sfml-graphics sfml-system sfml-main)
target_compile_features(panoramaviewer PUBLIC cxx_std_20)