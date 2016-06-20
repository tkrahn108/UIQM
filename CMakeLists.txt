project("UIQM")
cmake_minimum_required(VERSION 2.8)

Set(BINARY_NAME ${PROJECT_NAME})
aux_source_directory(./src SRC_LIST)

find_package( OpenCV REQUIRED )

add_executable(${BINARY_NAME} ${SRC_LIST} )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries( "${PROJECT_NAME}" ${OpenCV_LIBS} )
