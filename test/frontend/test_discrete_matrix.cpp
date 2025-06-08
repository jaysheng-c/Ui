/**
  ********************************************************************************
  * @file           : test_discrete_matrix.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/08
  * @version        : 1.0
  ********************************************************************************
  */
#include <QApplication>
#include <QPushButton>

#include "frontend/table/data/discrete_matrix.h"
#include "frontend/table/data/matrix.h"
#include "frontend/table/data/table_data.h"

QString alpha(const int section)
{
    QString colName;
    static QVector<QChar> alpha = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};
    int num2 = section + 1;
    while (num2 > 0) {
        num2 = num2 - 1;
        colName = alpha[num2 % alpha.size()] + colName;
        num2 /= static_cast<int>(alpha.size());
    }
    return colName;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    constexpr int tenThousand = 10000;
    constexpr int rows = 100 * tenThousand;
    constexpr int columns = 20;
    DiscreteMatrix<TableData> matrix(rows, columns);
    // Matrix<TableData> matrix(rows, columns);


    QPushButton btn;
    btn.show();

    QThreadPool::globalInstance()->start([&matrix, rows, columns]() {
        qDebug() << "start";
        QStringList headers(columns);
        for (int col{0}; col < columns; ++col) {
            headers[col] = alpha(col);
        }
        QElapsedTimer timer;
        timer.start();
        for (int col{0}; col < columns; ++col) {
            const auto &name = headers[col];
            for (int row {0}; row < rows; ++row) {
                matrix.setData(row, col, std::move(TableData(name + QString::number(row))));
            }
            qDebug() << name << "done";
        }
        qDebug() << "set data cost:" << timer.elapsed() << "ms";

        qDebug() << "start remove columns";
        timer.restart();
        qDebug() << "removeColumns:" << matrix.removeColumns(2, 16);
        qDebug() << "removeRows:" << matrix.removeRows(10, 80);
        qDebug() << "remove columns cost:" << timer.elapsed() << "ms";

        // qDebug() << headers.join('\t');
        // qDebug() << "";
        // for (int row{0}; row < matrix.rows(); ++row) {
        //     QStringList str(matrix.columns());
        //     for (int col{0}; col < matrix.columns(); ++col) {
        //         str[col] = matrix.at(row, col).data(TableData::Display).toString();
        //     }
        //     qDebug() << str.join('\t');
        // }

    });

    return QApplication::exec();
}