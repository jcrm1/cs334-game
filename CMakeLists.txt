cmake_minimum_required(VERSION 3.23)

project(GraphicsClassGame)

add_executable(Game)

target_sources(Game PRIVATE
  src/main.c
)

add_subdirectory(externals/glfw)
add_subdirectory(externals/glad)

target_link_libraries(Game glfw)
target_link_libraries(Game glad)

