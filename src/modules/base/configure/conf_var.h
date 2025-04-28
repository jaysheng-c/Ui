/**
  ********************************************************************************
  * @file           : conf_var.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef CONF_VAR_H
#define CONF_VAR_H

#include <QSharedPointer>
#include <QReadWriteLock>

#include "base/log.h"
#include "transformer.h"

class Var {
public:
    using ptr = QSharedPointer<Var>;

    explicit Var(QString name, QString desc = "") : m_name(std::move(name)), m_desc(std::move(desc)) {}
    virtual ~Var() = default;

    NODISCARD virtual const QString &name() const { return m_name; }
    NODISCARD virtual const QString &description() const { return m_desc; }
    NODISCARD virtual QString toString() const = 0;
    NODISCARD virtual QString typeName() const = 0;
    virtual bool fromString(const QString &val) = 0;

    NODISCARD int type() const { return m_type; }
    void setType(int type) { m_type = type; }

protected:
    QString m_name;
    QString m_desc;
    int m_type { 0 };
};

template <class T, class FromStr = Transformer<QString, T>, class ToStr = Transformer<T, QString>>
class ConfVar : public Var {
public:
    using ptr = QSharedPointer<ConfVar>;
    explicit ConfVar(const QString &name, const T &default_val, const QString &desc = "")
            : Var(name, desc), m_value(default_val) {}

    QString toString() const override
    {
        try {
            QReadLocker locker(&m_rwLock);
            return ToStr()(m_value);
        } catch (const std::exception &e) {
            qWarning() << "exception [" + QString(e.what()) + "] convert to string name=" + m_name;
        }
        return {};
    }

    bool fromString(const QString &str) override
    {
        try {
            setValue(FromStr()(str));
        } catch (const std::exception &e) {
            qWarning() << "exception [" + QString(e.what()) + "] convert from string name=" + m_name
                          + " - " + str;
            return false;
        }
        return true;
    }

    const T &value() const
    {
        QReadLocker locker(&m_rwLock);
        return m_value;
    }

    void setValue(const T &value)
    {
        {
            QReadLocker locker(&m_rwLock);
            if (value == m_value) {
                return;
            }
        }
        QWriteLocker locker(&m_rwLock);
        m_value = value;
    }

    QString typeName() const override { return QString(typeid(T).name()); }

protected:
    mutable QReadWriteLock m_rwLock;
    T m_value;
};

#endif //CONF_VAR_H
