/**
  ********************************************************************************
  * @file           : log.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QMap>
#include <QHash>

#include "no_copy.h"

class QFile;

class Logger final : public QObject, NoCopy {
public:
    enum class State {
        Success,
        Failed,
        Existed,
        Inited,
    };
    Q_ENUM(State)

    static Logger &instance();

    NODISCARD const QString &basePath() const;

    State initLogger(const QString &basePath = "");
    int addLogger(const QString &moduleName, const QString &logName);
    int addLoggers(const QMap<QString, QString> &loggers);

private:
    Logger() = default;

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    bool m_isInit { false };
    QString m_basePath;
    QHash<QString, QFile *> m_loggers;
};


#endif //LOG_H
