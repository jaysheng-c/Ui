# 工具栏组件

set(ToolHeader
        ${CMAKE_CURRENT_LIST_DIR}/dock/dock_title_widget.h
        ${CMAKE_CURRENT_LIST_DIR}/tool_bar.h
)

set(ToolSource
        ${CMAKE_CURRENT_LIST_DIR}/dock/dock_title_widget.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tool_bar.cpp
)

set(ToolFile
        ${ToolHeader}
        ${ToolSource}
)