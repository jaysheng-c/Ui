/**
  ********************************************************************************
  * @file           : test_logger.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "base/log.h"
#include "base/configure/conf_mgr.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>

template<>
class Transformer<QString, QList<int>> {
public:
    QList<int> operator()(const QString &str) {
        auto array = QJsonDocument::fromJson(str.toUtf8()).array();
        if (array.isEmpty()) {
            return {};
        }
        QList<int> list(array.size(), std::numeric_limits<int>::quiet_NaN());
        for (int i = 0; i < array.size(); ++i) {
            list[i] = array[i].toInt();
        }
        return list;
    }
};

template<>
class Transformer<QList<int>, QString> {
public:
    QString operator()(const QList<int> &data) {
        QJsonArray array;
        for (const auto &item : data) {
            array.append(item);
        }
        return QJsonDocument(array).toJson(QJsonDocument::Compact);
    }
};

int main(int argc, char *argv[])
{
    Logger::instance().initLogger();
    QString confDir = QDir::currentPath();
    qDebug() << confDir;

    constexpr char conf1[] = "conf1";
    constexpr char conf2[] = "conf2";

    /** 初始化配置 **/
    ConfMgr::instance().addConfigures({
        {conf1, confDir + "/configure_1.json"},
        {conf2, confDir + "/configure_2.json"},
    });
    if (auto conf = ConfMgr::instance().lookup(conf1)) {
        conf->lookup<QString>("name", "");
        conf->lookup<int>("age", -1);
        conf->lookup<QList<int>>("nums", {});
        conf->lookup<bool>("sex", true);
        conf->lookup<QString>("description", "");
        conf->lookup<QString>("version", "");
    }
    if (auto conf = ConfMgr::instance().lookup(conf2)) {
        conf->lookup<QString>("name", "");
        conf->lookup<int>("age", -1);
        conf->lookup<QList<int>>("nums", {});
        conf->lookup<bool>("sex", true);
        conf->lookup<QString>("description", "");
        conf->lookup<QString>("version", "");
    }
    /** 测试加载配置文件 **/
    ConfMgr::instance().loadConfigure();
    if (auto conf = ConfMgr::instance().lookup(conf1)) {
        qDebug() << conf1;
        qDebug() << conf->lookup<QString>("name", "")->value();
        qDebug() << conf->lookup<int>("age", -1)->value();
        qDebug() << conf->lookup<bool>("sex", true)->value();
        qDebug() << conf->lookup<QString>("description", "")->value();
        qDebug() << conf->lookup<QString>("version", "")->value();
    }
    qDebug();
    if (auto conf = ConfMgr::instance().lookup(conf2)) {
        qDebug() << conf2;
        qDebug() << conf->lookup<QString>("name", "")->value();
        qDebug() << conf->lookup<int>("age", -1)->value();
        qDebug() << conf->lookup<QList<int>>("nums", {})->value();
        qDebug() << conf->lookup<bool>("sex", true)->value();
        qDebug() << conf->lookup<QString>("description", "")->value();
        qDebug() << conf->lookup<QString>("version", "")->value();
    }





    return 0;
}