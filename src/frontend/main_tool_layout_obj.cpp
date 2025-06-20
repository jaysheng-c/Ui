/**
  ********************************************************************************
  * @file           : main_layout_obj.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/17
  * @version        : 1.0
  ********************************************************************************
  */

#include "main_tool_layout_obj.h"
#include <QActionGroup>
#include <QFileSystemModel>
#include <QVBoxLayout>

#include "main_window.h"

#include "toolbar/dock/dock_title_widget.h"

#include "tree/tree_view.h"

namespace {

class DockWidget final : public QDockWidget {
public:
    explicit DockWidget(const QString &name, MainWindow *parent, QWidget *widget)
        : QDockWidget(parent)
    {
        this->setFloating(false);
        this->setAllowedAreas(Qt::LeftDockWidgetArea);
        this->setFeatures(NoDockWidgetFeatures);
        this->setTitleBarWidget(new QWidget(this));

        auto *dockTitle = new DockTitleWidget(this);
        dockTitle->setWindowTitle(name);
        dockTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        auto *verWidget = new QWidget(this);
        verWidget->setLayout(new QVBoxLayout(verWidget));
        verWidget->layout()->setContentsMargins(0, 0, 0, 0);
        verWidget->layout()->setSpacing(0);
        verWidget->layout()->addWidget(dockTitle);
        verWidget->layout()->addWidget(widget);
        this->setWidget(verWidget);

        (void) connect(dockTitle, &DockTitleWidget::closeClicked, [this]() { this->setVisible(false); });
    }
};

}

MainToolLayoutObj::MainToolLayoutObj(MainWindow *parent)
    : QObject(parent), m_parent(parent), m_toolBar(new ToolBar(parent)), m_actionGroup(new QActionGroup(m_toolBar))
{
    m_toolBar->setFixedWidth(40);
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);
    m_actionGroup->setExclusive(false);
}

void MainToolLayoutObj::init()
{
    auto createAction = [this](const QString &name, QWidget *widget, const bool checked = false) {
        auto *action = this->addAction(name, checked);
        auto *dock = new DockWidget(name, m_parent, widget);
        dock->setVisible(checked);
        if (checked) {
            m_parent->addDockWidget(Qt::LeftDockWidgetArea, dock);
        }
        m_actionGroup->addAction(action);
        m_map[action] = dock;
        (void) connect(dock, &QDockWidget::visibilityChanged, this, &MainToolLayoutObj::onDockVisibleChanged);
    };

    createAction("Project", new TreeView(m_parent), true);
    auto *dirTree = new TreeView(m_parent);
    auto *model = new QFileSystemModel(dirTree);
    dirTree->setModel(model);
    model->setRootPath(QDir::currentPath());
    createAction("File", dirTree);

    (void) connect(m_actionGroup, &QActionGroup::triggered, this, &MainToolLayoutObj::onActionTriggered);
}

QAction * MainToolLayoutObj::addAction(const QString &name, const bool checked) const
{
    auto *action = m_toolBar->addAction(QIcon(":/image/" + name.toLower() + ".png"), name);
    action->setCheckable(true);
    action->setChecked(checked);
    return action;
}

void MainToolLayoutObj::onDockVisibleChanged(const bool visible) const
{
    auto *dock = qobject_cast<QDockWidget *>(sender());
    if (!visible) {
        m_parent->removeDockWidget(dock);
        if (auto *action = m_map.key(dock)) {
            action->setChecked(false);
        }
    }
}

void MainToolLayoutObj::onActionTriggered(QAction *action) const
{
    auto *dock = m_map.value(action);
    if (!dock) {
        return;
    }
    const bool checked = action->isChecked();
    if (!checked) {
        m_parent->removeDockWidget(dock);
    } else {
        m_parent->addDockWidget(Qt::LeftDockWidgetArea, dock);
    }
    dock->setVisible(checked);
    const auto *group =action->actionGroup();
    for (auto *t : group->actions()) {
        if (t != action) {
            m_parent->removeDockWidget(m_map.value(t));
            t->setChecked(false);
        }
    }
}
