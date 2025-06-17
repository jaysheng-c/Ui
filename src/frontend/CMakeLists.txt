# ui组件

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(title/title.cmake)
include(toolbar/tool.cmake)
include(table/table.cmake)
include(tree/tree.cmake)

add_library(ui SHARED)
target_sources(ui PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/global/global.h
        ${CMAKE_CURRENT_LIST_DIR}/main_window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main_window.h
        ${CMAKE_CURRENT_LIST_DIR}/main_tool_layout_obj.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main_tool_layout_obj.h
        ${ToolFile}
        ${TitleFile}
        ${TableFile}
        ${TreeFile}
)
target_link_libraries(ui PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
target_compile_definitions(ui PRIVATE UI_LIBRARY)


set(PROJECT_QRC_SOURCE ${CMAKE_CURRENT_LIST_DIR}/resource/resource.qrc)
qt_add_resources(${PROJECT_QRC_SOURCE})
add_executable(frontend
        ${PROJECT_QRC_SOURCE}
        main.cpp
)
target_link_libraries(frontend PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ui
)