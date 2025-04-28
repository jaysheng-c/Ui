/**
  ********************************************************************************
  * @file           : conf_mgr.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "conf_mgr.h"

namespace {
QReadWriteLock g_rwLock;
}

ConfMgr &ConfMgr::instance()
{
    static ConfMgr instance;
    return instance;
}

int ConfMgr::addConfigure(const QString &name, const QString &file)
{
    return addConfigures({ { name, file } });
}

int ConfMgr::addConfigures(const QMap<QString, QString> &configures)
{
    if (configures.isEmpty()) {
        return 0;
    }
    QWriteLocker locker(&g_rwLock);
    int count = 0;
    for (auto iter = configures.begin(); iter != configures.end(); ++iter) {
        auto name = iter.key().toLower();
        auto file = iter.value();
        if (name.isEmpty() || file.isEmpty()) {
            continue;
        }
        if (m_configures.contains(name)) {
            continue;
        }
        m_configures.insert(name, QSharedPointer<Configure>::create(file));
        ++count;
    }
    return count;
}

int ConfMgr::loadConfigure()
{
    for (const auto &configure : m_configures) {
        qDebug() << configure->loadConfigure();
    }
    return 0;
}


Configure::ptr ConfMgr::lookup(const QString &name)
{
    QReadLocker locker(&g_rwLock);
    return m_configures.value(name.toLower(), nullptr);
}