cmake_minimum_required(VERSION 3.23)

project(GraphicsClassGame LANGUAGES C CXX)

add_executable(Game)
target_compile_features(Game PRIVATE cxx_std_23)

target_sources(Game PRIVATE
  src/main.cpp
  src/Shader.hpp
)

add_subdirectory(externals/glfw)
add_subdirectory(externals/glad)

target_link_libraries(Game glfw)
target_link_libraries(Game glad)

