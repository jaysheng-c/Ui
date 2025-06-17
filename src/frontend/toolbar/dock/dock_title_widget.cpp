/**
  ********************************************************************************
  * @file           : dock_title.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/17
  * @version        : 1.0
  ********************************************************************************
  */

#include "dock_title_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

DockTitleWidget::DockTitleWidget(QWidget *parent)
    : QFrame(parent)
{
    this->setFixedHeight(40);
    this->setMouseTracking(true);
}

void DockTitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    QFrame::mouseMoveEvent(event);
    m_closeHover = m_closeRect.contains(event->pos());
    update();
}

void DockTitleWidget::mousePressEvent(QMouseEvent *event)
{
    QFrame::mousePressEvent(event);
    this->setProperty("closePressed", m_closeHover);
}

void DockTitleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);
    if (const auto var = this->property("closePressed"); var.isValid()) {
        if (var.toBool()) {
            // 关闭窗口
            emit closeClicked();
        }
    }
}

void DockTitleWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    auto rect = this->rect();
    constexpr int margin = 3;
    rect.adjust(margin * 2, margin, -margin * 2, -margin);

    auto textRect = painter.fontMetrics().boundingRect(this->windowTitle());
    if (textRect.height() < rect.height()) {
        const auto size = QSize{textRect.width(), rect.height()};
        textRect.setTopLeft(rect.topLeft());
        textRect.setSize(size);
    }
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, this->windowTitle());

    {
        // 绘制关闭按钮
        constexpr char closeText[] = "—";
        auto closeRect = painter.fontMetrics().boundingRect(closeText);
        closeRect.adjust(-2 * margin, 0, 0, 2 * margin);
        const auto size = qMin(closeRect.width(), rect.height());
        const double div = rect.height() - size;
        closeRect.setTopLeft(QPoint(rect.topRight()) - QPoint(closeRect.width(), -qFloor(div / 2)));
        closeRect.setSize({size, size});
        if (const QColor bg = m_closeHover ? m_closeBackgroundHover : m_closeBackground; bg.isValid()) {
            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(bg);
            painter.drawRect(closeRect);
            painter.restore();
        }
        painter.drawText(closeRect, Qt::AlignCenter, QString::fromStdString(closeText));
        m_closeRect = closeRect;
    }
}
