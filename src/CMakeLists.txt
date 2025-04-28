set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src)

## 获取当前frontend目录下的所有子目录
#file(GLOB FRONTEND_SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/frontend/*")
## 遍历每个子目录并调用 add_subdirectory
#foreach(SUBDIRECTORY ${FRONTEND_SUBDIRECTORIES})
#    if(IS_DIRECTORY ${SUBDIRECTORY})
#        add_subdirectory(${SUBDIRECTORY})
#    endif()
#endforeach()

# 获取当前backend目录下的所有子目录
file(GLOB BACKEND_SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/backend/*")
# 遍历每个子目录并调用 add_subdirectory
foreach(SUBDIRECTORY ${BACKEND_SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIRECTORY})
        add_subdirectory(${SUBDIRECTORY})
    endif()
endforeach()

add_subdirectory(frontend)
add_subdirectory(modules)