/**
  ********************************************************************************
  * @file           : conf.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "configure.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Var::ptr Configure::lookupBase(const QString &name)
{
    QReadLocker locker(&m_rwLock);
    return m_dataMap.value(name, nullptr);
}

int Configure::loadConfigure()
{
    QFile file(m_configureFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 文件不存在
        return -1;
    }
    auto doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (doc.isEmpty()) {
        // 不是json格式
        return -2;
    }

    QVector<QPair<QString, QJsonValue>> dataList;
    if (doc.isObject()) {
        auto obj = doc.object();
        for (auto it {obj.begin()}; it != obj.end(); ++it) {
            const auto &value = it.value();
            dataList.append(qMakePair(it.key(), value));
        }
    }
    for (const auto &pair : dataList) {
        const auto &key = pair.first;
        const auto &value = pair.second;
        if (key.isEmpty()) {
            continue;
        }
        auto var = lookupBase(key.toLower());
        if (var) {
            QWriteLocker locker(&m_rwLock);
            switch (value.type()) {
                case QJsonValue::Bool: var->fromString(value.toBool(true) ? "true" : "false"); break;
                case QJsonValue::Double: var->fromString(QString::number(value.toDouble())); break;
                case QJsonValue::String: var->fromString(value.toString()); break;
                case QJsonValue::Object: var->fromString(QJsonDocument(value.toObject()).toJson()); break;
                case QJsonValue::Array: var->fromString(QJsonDocument(value.toArray()).toJson()); break;
                case QJsonValue::Null:
                case QJsonValue::Undefined:
                    break;
            }
            var->setType(value.type());
        }
    }
    return 0;
}

int Configure::updateFile()
{
    QJsonObject obj;
    for (auto it = m_dataMap.begin(); it != m_dataMap.end(); ++it) {
        switch (QJsonValue::Type(it.value()->type())) {
            case QJsonValue::Bool: obj[it.key()] = it.value()->toString() == "true"; break;
            case QJsonValue::Double: obj[it.key()] = it.value()->toString().toDouble(); break;
            case QJsonValue::String: obj[it.key()] = it.value()->toString(); break;
            case QJsonValue::Array:
                obj[it.key()] = QJsonDocument::fromJson(it.value()->toString().toUtf8()).array();
                break;
            case QJsonValue::Object:
                obj[it.key()] = QJsonDocument::fromJson(it.value()->toString().toUtf8()).object();
                break;
            case QJsonValue::Undefined:
            case QJsonValue::Null:
                break;
        }
    }
    QFile file(m_configureFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 文件不存在
        return -1;
    }
    file.write(QJsonDocument(obj).toJson());
    file.close();

    return 0;
}