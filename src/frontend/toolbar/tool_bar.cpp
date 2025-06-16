/**
  ********************************************************************************
  * @file           : tool_bar.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */

#include "tool_bar.h"
#include <QPainter>
#include <QToolButton>

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
{
    this->setObjectName("ToolBar");
    this->setFixedWidth(40);

    auto createAction = [this](const QString &name, const int type) -> QAction* {
        auto *action = new QAction(name, this);
        action->setObjectName(name);
        action->setCheckable(true);
        (void) connect(action, &QAction::triggered, [this, type](const bool checked) {
            emit triggered(checked, type);
        });
        return action;
    };

    this->addAction(createAction("file", 0));
    this->addAction(createAction("dir", 0));

    for (auto *i : this->children()) {
        if (auto *btn = qobject_cast<QToolButton*>(i)) {
            btn->setAutoExclusive(true);
        }
    }
}

void ToolBar::paintEvent(QPaintEvent *event)
{
    QToolBar::paintEvent(event);
    // auto rect = this->rect();
    // rect.adjust(0, 0, -1, 0);
    // QPainter painter(this);
    // painter.setPen(Qt::NoPen);
    // painter.setBrush(QBrush(QColor(43, 45, 48)));
    // painter.drawRect(rect);
}
