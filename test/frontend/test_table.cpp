/**
  ********************************************************************************
  * @file           : main.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */
#include <QApplication>
#include <QFile>
#include <QElapsedTimer>
#include <QTimer>
#include <QtZlib/zlib.h>
#include "frontend/main_window.h"
#include "frontend/table/data/table_data.h"
#include "frontend/table/data/matrix.h"
#include "frontend/table/table_view.h"
#include "frontend/table/table_model.h"

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

int testMatrixRemoveCol_ColumnType();

int testMatrixRemoveRow_RowType();

int testMatrixInsertCol_ColumnType();

int testMatrixInsertRow_RowType();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<TableData>("TableData"); // 显式注册类型

//    QVector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};
//    std::swap_ranges(vec.begin() + 4, vec.begin() + 6, vec.begin());
//    qDebug() << vec;
//    return 0;

//    return testMatrixRemoveCol_ColumnType();
//    return testMatrixRemoveRow_RowType();
//    return testMatrixInsertCol_ColumnType();
//    return testMatrixInsertRow_RowType();

    TableView table;
    const auto model = table.tableModel();

    QStringList colNames(20);
    for (int i{0}; i < colNames.size(); ++i) {
        colNames[i] = alpha(i);
    }
    QElapsedTimer timer;
    timer.start();
    constexpr int rows = 100 * 10000;
    constexpr int columns = 20;
    Matrix<TableData> data(rows, columns, ExpandType::Row);
    for (int row {0}; row < data.rows(); ++row) {
        for (int column {0}; column < data.columns(); ++column) {
            data.setData(row, column, TableData(colNames.at(column) + QString::number(row)));
        }
    }
    model->resetData(std::move(data));
    qDebug() << "write data row x column [" + QString::number(rows) + " x " + QString::number(columns) + "] cost time:"
             << timer.elapsed() << "ms";
    table.show();

    // // 序列化和反序列化
    // QTimer::singleShot(1000 * 5, [model]() {
    //     qDebug() << "start to serialize";
    //     QElapsedTimer seTimer;
    //     seTimer.restart();
    //     const auto d = model->serializeData();
    //     const qsizetype size = d.size();
    //     qDebug() << "serialize data row x column [" + QString::number(rows) + " x " + QString::number(columns) +
    //                 "] cost time:" << seTimer.elapsed() << "ms, data size:" << size << "bytes";
    //     model->resetData();
    //     QTimer::singleShot(1000 * 5, [model, d]() {
    //         qDebug() << "start to deserialize";
    //         QElapsedTimer desTimer;
    //         desTimer.start();
    //         model->deserialize(d);
    //         qDebug() << "deserialize data row x column [" + QString::number(rows) + " x " + QString::number(columns) +
    //                     "] cost time:" << desTimer.elapsed() << "ms";
    //     });
    // });

    return QApplication::exec();
}

int testMatrixRemoveCol_ColumnType()
{
    constexpr int columns = 20;
    constexpr int rows = 100 * 10000;
    constexpr ExpandType type = ExpandType::Column;
    std::shared_ptr<Matrix<QString>> mat = std::make_shared<Matrix<QString>>(rows, columns, type);
    QElapsedTimer timer;
    timer.start();
    for (auto column{0}; column < columns; ++column) {
        auto data = alpha(column);
        for (auto row{0}; row < rows; ++row) {
            QString s = data + QString::number(row);
            s.resize(8, ' ');
            mat->setData(row, column, s);
        }
    }
    qDebug() << "write data(row x column: " + QString::number(rows) + "x" + QString::number(columns) + ") cost:"
            << timer.elapsed() << "ms";
    timer.restart();


    // row and column operator(insert / remove)
    constexpr int count = 10;
    qDebug() << "remove" << count << "columns res:" << mat->removeColumns(0, count);
    qDebug() << "remove" << count << "columns cost:" << timer.elapsed() << "ms";


    timer.restart();
    QStringList list(mat->rows(), {});
    for (auto row{0}; row < mat->rows(); ++row) {
        QString rowData;
        for (auto column{0}; column < mat->columns(); ++column) {
            rowData += mat->at(row, column);
        }
        list[row] = std::move(rowData);
    }
    qDebug() << "read data: " << timer.elapsed() << "ms";
    timer.restart();
    QFile file(QString::fromUtf8(__FUNCTION__) + ".txt");
    qDebug() << "open file:" << file.open(QIODevice::WriteOnly);
    file.write(list.join("\n").toUtf8());
    file.close();
    qDebug() << "write file: " << timer.elapsed() << "ms";

    return 0;
}

int testMatrixRemoveRow_RowType()
{
    constexpr int columns = 100 * 10000;
    constexpr int rows = 20;
    constexpr ExpandType type = ExpandType::Row;
    std::shared_ptr<Matrix<QString> > mat = std::make_shared<Matrix<QString> >(rows, columns, type);
    QElapsedTimer timer;
    timer.start();
    for (auto column{0}; column < columns; ++column) {
        auto data = alpha(column);
        for (auto row{0}; row < rows; ++row) {
            QString s = data + QString::number(row);
            s.resize(8, ' ');
            mat->setData(row, column, s);
        }
    }
    qDebug() << "write data(row x column: " + QString::number(rows) + "x" + QString::number(columns) + ") cost:"
            << timer.elapsed() << "ms";
    timer.restart();

    // row and column operator(insert / remove)
    constexpr int count = 10;
    qDebug() << "remove" << count << "rows res:" << mat->removeRows(0, count);
    qDebug() << "remove" << count << "rows cost:" << timer.elapsed() << "ms";


    timer.restart();
    QStringList list(mat->columns(), {});
    for (auto column{0}; column < mat->columns(); ++column) {
        QString columnData;
        for (auto row{0}; row < mat->rows(); ++row) {
            columnData += mat->at(row, column);
        }
        list[column] = std::move(columnData);
    }
    qDebug() << "read data: " << timer.elapsed() << "ms";
    timer.restart();
    QFile file(QString::fromUtf8(__FUNCTION__) + ".txt");
    qDebug() << "open file:" << file.open(QIODevice::WriteOnly);
    file.write(list.join("\n").toUtf8());
    file.close();
    qDebug() << "write file: " << timer.elapsed() << "ms";

    return 0;
}

int testMatrixInsertCol_ColumnType()
{
    constexpr int columns = 20;
    constexpr int rows = 100 * 10000;
    constexpr ExpandType type = ExpandType::Column;
    std::shared_ptr<Matrix<QString> > mat = std::make_shared<Matrix<QString> >(rows, columns, type);
    QElapsedTimer timer;
    timer.start();
    for (auto column{0}; column < columns; ++column) {
        auto data = alpha(column);
        for (auto row{0}; row < rows; ++row) {
            QString s = data + QString::number(row);
            s.resize(8, ' ');
            mat->setData(row, column, s);
        }
    }
    qDebug() << "write data(row x column: " + QString::number(rows) + "x" + QString::number(columns) + ") cost:"
            << timer.elapsed() << "ms";
    timer.restart();


    // row and column operator(insert / remove)
    constexpr int count = 10;
    qDebug() << "insert" << count << "columns res:" << mat->insertColumns(1, count, "1_____2 ");
    qDebug() << "insert" << count << "columns cost:" << timer.elapsed() << "ms";


    timer.restart();
    QStringList list(mat->rows(), {});
    for (auto row{0}; row < mat->rows(); ++row) {
        QString rowData;
        for (auto column{0}; column < mat->columns(); ++column) {
            rowData += mat->at(row, column);
        }
        list[row] = std::move(rowData);
    }
    qDebug() << "read data: " << timer.elapsed() << "ms";
    timer.restart();
    QFile file(QString::fromUtf8(__FUNCTION__) + ".txt");
    qDebug() << "open file:" << file.open(QIODevice::WriteOnly);
    file.write(list.join("\n").toUtf8());
    file.close();
    qDebug() << "write file: " << timer.elapsed() << "ms";

    return 0;
}

int testMatrixInsertRow_RowType()
{
    constexpr int columns = 100 * 10000;
    constexpr int rows = 20;
    constexpr ExpandType type = ExpandType::Row;
    std::shared_ptr<Matrix<QString> > mat = std::make_shared<Matrix<QString> >(rows, columns, type);
    QElapsedTimer timer;
    timer.start();
    for (auto column{0}; column < columns; ++column) {
        auto data = alpha(column);
        for (auto row{0}; row < rows; ++row) {
            QString s = data + QString::number(row);
            s.resize(8, ' ');
            mat->setData(row, column, s);
        }
    }
    qDebug() << "write data(row x column: " + QString::number(rows) + "x" + QString::number(columns) + ") cost:"
            << timer.elapsed() << "ms";
    timer.restart();

    // row and column operator(insert / remove)
    constexpr int count = 10;
    qDebug() << "insert" << count << "rows res:" << mat->insertRows(19, count, "1_____2 ");
    qDebug() << "insert" << count << "rows cost:" << timer.elapsed() << "ms";


    timer.restart();
    QStringList list(mat->columns(), {});
    for (auto column{0}; column < mat->columns(); ++column) {
        QString columnData;
        for (auto row{0}; row < mat->rows(); ++row) {
            columnData += mat->at(row, column);
        }
        list[column] = std::move(columnData);
    }
    qDebug() << "read data: " << timer.elapsed() << "ms";
    timer.restart();
    QFile file(QString::fromUtf8(__FUNCTION__) + ".txt");
    qDebug() << "open file:" << file.open(QIODevice::WriteOnly);
    file.write(list.join("\n").toUtf8());
    file.close();
    qDebug() << "write file: " << timer.elapsed() << "ms";

    return 0;
}
