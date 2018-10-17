project(bild)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Ofast")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(FreeImage REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${FREEIMAGE_INCLUDE_DIRS}
)

set(bild_SOURCES
  src/signal.c
  src/image.c
  src/quantize.c
  src/bild.c
  src/wavelet.c
  src/decomposition.c
  src/rle.c
  src/huffman.c
  src/pack.c
  src/main.c
)

add_executable(bild ${bild_SOURCES})

target_link_libraries(bild
  ${FREEIMAGE_LIBRARIES}
)
