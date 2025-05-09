/**
  ********************************************************************************
  * @file           : table_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/09
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_cmd.h"
#include <QDebug>

Reflector::~Reflector()
{
  for (const auto& objectFactory : m_objectFactories) {
      delete objectFactory;
  }
  m_objectFactories.clear();
}

void Reflector::Register(const QString &name, ObjectFactory *obj)
{
    if (m_objectFactories.contains(name)) {
        qWarning() << "class [" + name + "] has been registered!";
        return;
    }
    m_objectFactories[name] = obj;
}

TableCmd *Reflector::GetNewInstance(const QString &name, TableView *parent) const
{
    if (auto *obj = m_objectFactories.value(name)) {
        return obj->newInstance(parent);
    }
    qWarning() << "class [" + name + "] not registered!";
    return nullptr;
}

Reflector &Reflector::GetInstance()
{
    static Reflector reflector;
    return reflector;
}
