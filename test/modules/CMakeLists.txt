set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test/modules)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test/modules)

add_executable(TestLogger test_logger.cpp)
target_link_directories(TestLogger PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(TestLogger PRIVATE
        Qt::Core
        base
)
# 复制 guide 库到当前路径
add_custom_command(TARGET TestLogger POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/${CMAKE_SHARED_LIBRARY_PREFIX}base${CMAKE_SHARED_LIBRARY_SUFFIX}
        $<TARGET_FILE_DIR:TestLogger>
)