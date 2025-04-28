/**
  ********************************************************************************
  * @file           : log.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "log.h"
#include <iostream>
#include <QDir>
#include <QFile>
#include <QReadWriteLock>

namespace {
QReadWriteLock g_rwLock;
}

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString prefix;
    switch (type) {
        case QtDebugMsg: prefix = "[Debug] "; break;
        case QtInfoMsg: prefix = "[Info] "; break;
        case QtWarningMsg: prefix = "[Warning] "; break;
        case QtCriticalMsg: prefix = "[Critical] "; break;
        case QtFatalMsg: prefix = "[Fatal] "; break;
    }

    QReadLocker locker(&g_rwLock);
    if (Logger::instance().m_isInit) {
        if (auto file = Logger::instance().m_loggers.value(context.category)) {
            QTextStream out(file);
            out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << "] "
#ifdef QT_DEBUG
                << context.file << ":" << context.line << " "
#endif
                << prefix << context.function << ": " << msg << "\n";
        }
    }
#ifdef QT_DEBUG
    switch (type) {
        case QtDebugMsg:
        case QtInfoMsg:
            std::cout << context.file << ":" << context.line << " " << qPrintable(prefix)
                      << context.function << ": " << qPrintable(msg) << std::endl;
            break;
        default:
            std::cerr << context.file << ":" << context.line << " " << qPrintable(prefix)
                      << context.function << ": " << qPrintable(msg) << std::endl;
            break;
    }
#endif
    if (type == QtFatalMsg) {
        abort();
    }
}

const QString &Logger::basePath() const
{
    QReadLocker locker(&g_rwLock);
    return m_basePath;
}

Logger::State Logger::initLogger(const QString &basePath)
{
    QWriteLocker locker(&g_rwLock);
    if (m_isInit) {
        return State::Inited;
    }
    m_isInit = true;
    m_basePath = basePath.isEmpty() ? QDir::currentPath() : basePath;
    qInstallMessageHandler(Logger::messageHandler);
    return State::Success;
}

int Logger::addLogger(const QString &moduleName, const QString &logName)
{
    return addLoggers({ {moduleName, logName} });
}

int Logger::addLoggers(const QMap<QString, QString> &loggers)
{
    QWriteLocker locker(&g_rwLock);
    int success { 0 };
    for (auto it = loggers.begin(); it != loggers.end(); ++it) {
        if (m_loggers.contains(it.key())) {
            continue;
        }
        QFileInfo fileInfo(it.value());
        auto file = new QFile(m_basePath + "/" + fileInfo.fileName(), this);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            file->close();
            file->deleteLater();
            continue;
        }
        m_loggers[it.key()] = file;
        ++success;
    }
    return success;
}