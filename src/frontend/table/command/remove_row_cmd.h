/**
  ********************************************************************************
  * @file           : remove_row_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef REMOVE_ROW_CMD_H
#define REMOVE_ROW_CMD_H

#include "table_cmd.h"

class RemoveRowCmd final : public TableCmd {
public:
    explicit RemoveRowCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};


#endif //REMOVE_ROW_CMD_H
