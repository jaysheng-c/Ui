/**
  ********************************************************************************
  * @file           : dock_title.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/17
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef DOCK_TITLE_WIDGET_H
#define DOCK_TITLE_WIDGET_H

#include <QFrame>

class DockTitleWidget final : public QFrame {
Q_OBJECT
    Q_PROPERTY(bool closeHover MEMBER m_closeHover)
    Q_PROPERTY(QColor closeBackground MEMBER m_closeBackground)
    Q_PROPERTY(QColor closeBackground_hover MEMBER m_closeBackgroundHover)
signals:
    void closeClicked();
public:
    explicit DockTitleWidget(QWidget *parent = nullptr);
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_closeHover;
    QRect m_closeRect;
    QColor m_closeBackground;
    QColor m_closeBackgroundHover;
};


#endif //DOCK_TITLE_WIDGET_H
