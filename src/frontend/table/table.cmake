# 表格组件

set(TableHeader
        ${CMAKE_CURRENT_LIST_DIR}/table_global.h
        ${CMAKE_CURRENT_LIST_DIR}/command/table_cmd.h
        ${CMAKE_CURRENT_LIST_DIR}/command/insert_row_cmd.h
        ${CMAKE_CURRENT_LIST_DIR}/command/insert_column_cmd.h
        ${CMAKE_CURRENT_LIST_DIR}/command/remove_row_cmd.h
        ${CMAKE_CURRENT_LIST_DIR}/command/remove_column_cmd.h
        ${CMAKE_CURRENT_LIST_DIR}/command/paste_cmd.h

        ${CMAKE_CURRENT_LIST_DIR}/data/table_data.h
        ${CMAKE_CURRENT_LIST_DIR}/data/copy_data.h

        ${CMAKE_CURRENT_LIST_DIR}/table_delegate.h
        ${CMAKE_CURRENT_LIST_DIR}/table_model.h
        ${CMAKE_CURRENT_LIST_DIR}/table_view.h
        ${CMAKE_CURRENT_LIST_DIR}/table_menu.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/insert_child_dlg.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/remove_child_dlg.h
)

set(TableSource
        ${CMAKE_CURRENT_LIST_DIR}/command/table_cmd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command/insert_row_cmd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command/insert_column_cmd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command/remove_row_cmd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command/remove_column_cmd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command/paste_cmd.cpp

        ${CMAKE_CURRENT_LIST_DIR}/data/table_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/data/copy_data.cpp

        ${CMAKE_CURRENT_LIST_DIR}/table_delegate.cpp
        ${CMAKE_CURRENT_LIST_DIR}/table_model.cpp
        ${CMAKE_CURRENT_LIST_DIR}/table_view.cpp
        ${CMAKE_CURRENT_LIST_DIR}/table_menu.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/insert_child_dlg.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/remove_child_dlg.cpp
)

set(TableFile
        ${TableHeader}
        ${TableSource}
)