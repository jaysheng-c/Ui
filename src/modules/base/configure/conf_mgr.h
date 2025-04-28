/**
  ********************************************************************************
  * @file           : conf_mgr.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef CONF_MGR_H
#define CONF_MGR_H


#include "configure.h"

class ConfMgr : public NoCopy {
public:
    static ConfMgr &instance();

    int addConfigure(const QString &name, const QString &file);
    int addConfigures(const QMap<QString, QString> &configures);
    int loadConfigure();
    Configure::ptr lookup(const QString &name);

private:
    ConfMgr() = default;

    QMap<QString, Configure::ptr> m_configures;
};


#endif //CONF_MGR_H
