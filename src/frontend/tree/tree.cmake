# 树组件

set(TreeHeader
        ${CMAKE_CURRENT_LIST_DIR}/tree_view.h
        ${CMAKE_CURRENT_LIST_DIR}/tree_model.h
        ${CMAKE_CURRENT_LIST_DIR}/tree_menu.h
)

set(TreeSource
        ${CMAKE_CURRENT_LIST_DIR}/tree_view.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tree_model.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tree_menu.cpp
)

set(TreeFile
        ${TreeHeader}
        ${TreeSource}
)