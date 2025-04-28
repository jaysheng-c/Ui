add_library(base SHARED)

set(BaseHeaders
        ${CMAKE_CURRENT_LIST_DIR}/log.h
        ${CMAKE_CURRENT_LIST_DIR}/configure/transformer.h
        ${CMAKE_CURRENT_LIST_DIR}/configure/conf_var.h
        ${CMAKE_CURRENT_LIST_DIR}/configure/configure.h
        ${CMAKE_CURRENT_LIST_DIR}/configure/conf_mgr.h
)

set(BaseSources
        ${CMAKE_CURRENT_LIST_DIR}/log.cpp
        ${CMAKE_CURRENT_LIST_DIR}/configure/configure.cpp
        ${CMAKE_CURRENT_LIST_DIR}/configure/conf_mgr.cpp
)

target_sources(base PRIVATE
        ${BaseHeaders}
        ${BaseSources}
)
target_link_libraries(base PRIVATE
        Qt::Core
)