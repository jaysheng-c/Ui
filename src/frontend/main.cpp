/**
  ********************************************************************************
  * @file           : main.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/15
  * @version        : 1.0
  ********************************************************************************
  */
#include <QApplication>
#include <QFile>
#include <QResource>

#include "main_window.h"

class LoadQss {
public:
    int operator()() const
    {
        QFile file(":/Qss/main.qss");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << file.errorString() << file.fileName();
            return -1;
        }
        auto *app = qobject_cast<QApplication*>(QApplication::instance());
        app->setStyleSheet(file.readAll());
        file.close();
        return 0;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int ret = 0;

    // 加载qss
    ret = LoadQss()();
    if (ret != 0) {
        return ret;
    }

    MainWindow w;
    w.resize(800, 600);
    w.show();

    ret = QApplication::exec();
    return ret;
}