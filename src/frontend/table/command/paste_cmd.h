/**
  ********************************************************************************
  * @file           : paste_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef PASTE_CMD_H
#define PASTE_CMD_H

#include "table_cmd.h"

class PasteCmd final : public TableCmd {
public:
    explicit PasteCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;

private:
    void tableCmd(const QItemSelection &selectionItem) const;
};


#endif //PASTE_CMD_H
