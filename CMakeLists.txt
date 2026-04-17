cmake_minimum_required(VERSION 3.23)

include(FetchContent)

project(GraphicsClassGame LANGUAGES C CXX)

add_executable(Game)
target_compile_features(Game PRIVATE cxx_std_23)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

target_sources(Game PRIVATE
  src/main.cpp
  src/Shader.hpp
  src/Camera.hpp
  src/stb_image.h
  src/Constants.hpp
)

add_subdirectory(externals/glfw)
add_subdirectory(externals/glad)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed # refs/tags/1.0.3
)

FetchContent_MakeAvailable(glm)

target_link_libraries(Game glfw)
target_link_libraries(Game glad)
target_link_libraries(Game glm::glm)
