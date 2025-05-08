set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test)

include_directories(${CMAKE_SOURCE_DIR}/src/modules)
link_directories(${CMAKE_BINARY_DIR}/src)

# 获取当前目录下的所有子目录
file(GLOB SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*")

# 遍历每个子目录并调用 add_subdirectory
foreach(SUBDIRECTORY ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIRECTORY})
        add_subdirectory(${SUBDIRECTORY})
    endif()
endforeach()