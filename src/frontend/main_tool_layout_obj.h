/**
  ********************************************************************************
  * @file           : main_tool_layout_obj.h
  * @author         : jaysheng
  * @brief          : 主要工具布局对象
  * @attention      : None
  * @date           : 2025/06/17
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MAIN_TOOL_LAYOUT_OBJ_H
#define MAIN_TOOL_LAYOUT_OBJ_H

#include <QDockWidget>
#include <QObject>
#include <QPointer>

#include "toolbar/tool_bar.h"

class MainWindow;

class MainToolLayoutObj final : public QObject {
public:
    explicit MainToolLayoutObj(MainWindow *parent);
    ToolBar *toolBar() const { return m_toolBar; }

    void init();

private:
    QAction *addAction(const QString &name, bool checked = false) const;

    void onDockVisibleChanged(bool visible) const;
    void onActionTriggered(QAction *action) const;

    QPointer<MainWindow> m_parent;
    QPointer<ToolBar> m_toolBar;
    QPointer<QActionGroup> m_actionGroup;
    QHash<QAction*, QDockWidget*> m_map;
};



#endif //MAIN_TOOL_LAYOUT_OBJ_H
