/**
  ********************************************************************************
  * @file           : conf.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QObject>

#include "conf_var.h"

class Configure : public QObject, public NoCopy {
public:
    using ptr = QSharedPointer<Configure>;
    using VarMap = QMap<QString, Var::ptr>;

    enum class State {
        Success,
        NoExisted,
        FormatError,
    };
    Q_ENUM(State)


    explicit Configure(QString file) : m_configureFile(std::move(file)) {}
    ~Configure() override = default;


    template<class T>
    typename ConfVar<T>::ptr lookup(const QString &name, const T &defaultValue, const QString &desc = "")
    {
        QWriteLocker locker(&m_rwLock);
        if (m_dataMap.contains(name)) {
            auto tmp = m_dataMap.value(name).dynamicCast<ConfVar<T>>();
            if (tmp) {
                return tmp;
            } else {
                return nullptr;
            }
        }
        auto stdName = name.toStdString();
        if(stdName.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
            qWarning() << "Lookup name invalid " << name;
            throw std::invalid_argument(stdName);
        }
        auto v = QSharedPointer<ConfVar<T>>::create(name, defaultValue, desc);
        m_dataMap.insert(name, v);
        return v;
    }

    template<class T>
    typename ConfVar<T>::ptr lookup(const QString &name) {
        auto var = lookupBase(name);
        if (var) {
            return var.dynamicCast<ConfVar<T>>();
        }
        return nullptr;
    }

    Var::ptr lookupBase(const QString &name);
    virtual int loadConfigure();
    virtual int updateFile();

private:
    QReadWriteLock m_rwLock;
    VarMap m_dataMap;
    QString m_configureFile;
};


#endif //CONFIGURE_H
