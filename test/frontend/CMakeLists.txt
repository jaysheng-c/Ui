set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test/frontend)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test/frontend)

add_executable(TestUi test_ui.cpp)
target_link_directories(TestUi PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(TestUi PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ui
)
# 复制 guide 库到当前路径
add_custom_command(TARGET TestUi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/${CMAKE_SHARED_LIBRARY_PREFIX}ui${CMAKE_SHARED_LIBRARY_SUFFIX}
        $<TARGET_FILE_DIR:TestUi>
)