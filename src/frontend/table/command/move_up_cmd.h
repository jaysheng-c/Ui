/**
  ********************************************************************************
  * @file           : move_up_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/17
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MOVE_UP_CMD_H
#define MOVE_UP_CMD_H

#include "table_cmd.h"

class MoveUpCmd final : public TableCmd {
public:
  explicit MoveUpCmd(TableView *table);
  void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //MOVE_UP_CMD_H
