/**
  ********************************************************************************
  * @file           : remove_column_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef REMOVE_COLUMN_CMD_H
#define REMOVE_COLUMN_CMD_H

#include "table_cmd.h"

class RemoveColumnCmd final : public TableCmd {
public:
    explicit RemoveColumnCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};


#endif //REMOVE_COLUMN_CMD_H
