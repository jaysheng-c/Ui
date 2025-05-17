/**
  ********************************************************************************
  * @file           : table_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_CMD_H
#define TABLE_CMD_H

#include <QMap>
#include <QVector>
#include <QPointer>
#include "frontend/global/global.h"

class TableView;
class QObject;
class QItemSelection;

class UI_EXPORT TableCmd {
public:
    struct Range {
        int left;
        int right;
        int top;
        int bottom;
        friend QDebug operator<<(QDebug debug, const Range &range);
    };
    explicit TableCmd(TableView *table);
    virtual ~TableCmd() = default;

    virtual void cmd(QObject *contextObject, const QItemSelection &selectionItem) = 0;

protected:
    QPointer<TableView> m_table;
};

template<class T>
class Greater {
public:
    QVector<T> operator()(const QVector<T> &data) const
    {
        auto d = data;
        std::sort(d.begin(), d.end(), std::greater<T>());
        return d;
    }
};

template<class T>
class Smaller {
public:
    QVector<T> operator()(const QVector<T> &data) const
    {
        auto d = data;
        std::sort(d.begin(), d.end(), std::less<T>());
        return d;
    }
};

// 创建反射类的工厂
class ObjectFactory {
public:
    ObjectFactory() = default;
    virtual ~ObjectFactory() = default;
    virtual TableCmd *newInstance(TableView *parent) = 0;
};

// 反射管理类
class Reflector {
public:
    Reflector() = default;
    ~Reflector();
    void Register(const QString &name, ObjectFactory *obj);
    TableCmd *GetNewInstance(const QString &name, TableView *parent) const;
    static Reflector &GetInstance();
private:
    QMap<QString, ObjectFactory *> m_objectFactories;
};

// 获取已注册反射类实列对象
template<class T>
std::shared_ptr<T> GetNewCmdInstance(const QString &name, TableView *parent)
{
    return std::shared_ptr<T>(Reflector::GetInstance().GetNewInstance(name, parent));
}

// 注册反射类宏
#define REFLECT(name)                                   \
class ObjectFactory_##name : public ObjectFactory {     \
public:                                                 \
    ObjectFactory_##name() {}                           \
    virtual ~ObjectFactory_##name() {}                  \
    TableCmd *newInstance(TableView *parent) override   \
    {                                                   \
        return new name(parent);                        \
    }                                                   \
protected:                                              \
};                                                      \
class Register_##name {                                 \
public:                                                                             \
    Register_##name() {                                                             \
        Reflector::GetInstance().Register(#name, new ObjectFactory_##name());       \
    }                                                                               \
};                                                                                  \
Register_##name register_##name;

#endif //TABLE_CMD_H
