/**
  ********************************************************************************
  * @file           : main_window.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "global/global.h"

class MainToolLayoutObj;

class UI_EXPORT MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QPointer<MainToolLayoutObj> m_mainToolLayoutObj;
};


#endif //MAIN_WINDOW_H
