cmake_minimum_required(VERSION 3.8)
project(micro_sockets
  VERSION
    0.1.0
  DESCRIPTION
    "easy tcp/udp socket communication"
  LANGUAGES
    C
)

add_library(micro_sockets INTERFACE)
target_include_directories(micro_sockets INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS micro_sockets
  PUBLIC_HEADER
    DESTINATION include
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

add_subdirectory(examples)
