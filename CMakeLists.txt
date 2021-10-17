cmake_minimum_required(VERSION 3.9.6)

project(vncs)

find_package(PkgConfig REQUIRED)

pkg_check_modules(uv REQUIRED libuv)

add_library(vncs
  include/vncs/client_list.hpp
  include/vncs/server.hpp
  include/vncs/app.hpp
  include/vncs/app_factory.hpp
  src/client.hpp
  src/client.cpp
  src/client_list.cpp
  src/pixel_format.hpp
  src/pixel_format.cpp
  src/messages.hpp
  src/message_parser.hpp
  src/message_parser.cpp
  src/string_view.hpp
  src/string_view.cpp
  src/server.cpp
  src/null_security.hpp
  src/null_security.cpp)

target_link_libraries(vncs
  PUBLIC
    ${uv_LIBRARIES})

target_include_directories(vncs
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${uv_INCLUDE_DIRS}")

add_executable(main main.cpp)

target_link_libraries(main PRIVATE glfw vncs)
