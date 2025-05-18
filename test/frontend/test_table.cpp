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

QByteArray compressData(const QByteArray &data) {
    QByteArray compressed;
    constexpr int BUFSIZE = 128 * 1024;
    char buf[BUFSIZE];

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    deflateInit(&strm, Z_DEFAULT_COMPRESSION);

    strm.avail_in = data.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));

    do {
        strm.avail_out = BUFSIZE;
        strm.next_out = reinterpret_cast<Bytef*>(buf);
        deflate(&strm, Z_FINISH);
        compressed.append(buf, BUFSIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    deflateEnd(&strm);
    return compressed;
}


QByteArray decompressData(const QByteArray &compressed) {
    QByteArray decompressed;
    constexpr int BUFSIZE = 128 * 1024;
    char buf[BUFSIZE];

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    inflateInit(&strm);

    strm.avail_in = compressed.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed.data()));

    int ret;
    do {
        strm.avail_out = BUFSIZE;
        strm.next_out = reinterpret_cast<Bytef*>(buf);
        ret = inflate(&strm, Z_NO_FLUSH);
        decompressed.append(buf, BUFSIZE - strm.avail_out);
    } while (ret == Z_OK);

    inflateEnd(&strm);
    return decompressed;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<TableData>("TableData"); // 显式注册类型
    // {
    //     // 压缩
    //     constexpr qsizetype count = 100 * 10000;
    //     QByteArray byte;
    //     QDataStream ds(&byte, QIODevice::WriteOnly);
    //     QElapsedTimer toStreamTimer;
    //     toStreamTimer.start();
    //     for (qsizetype i{0}; i < count; ++i) {
    //         TableData tData(QString::number(i + 1));
    //         ds << tData;
    //     }
    //     qDebug() << "write " + QString::number(count) + " times, cost:" << toStreamTimer.elapsed() << "ms";
    //     qDebug() << "origin size:" << byte.size();
    //     toStreamTimer.restart();
    //     const auto comByte = compressData(byte);
    //     qDebug() << "compress cost:" << toStreamTimer.elapsed() << "ms";
    //     qDebug() << "after compress size:" << comByte.size();
    //
    //     QFile file(QString::fromUtf8(__FUNCTION__) + ".txt");
    //     qDebug() << "open file:" << file.open(QIODevice::WriteOnly);
    //     file.write(comByte);
    //     file.close();
    // }

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
    Matrix<TableData> data(/*10000 **/ 100, 20, ExpandType::Row);
    for (int row {0}; row < data.rows(); ++row) {
        for (int column {0}; column < data.columns(); ++column) {
            data.setData(row, column, TableData(colNames.at(column) + QString::number(row)));
        }
    }
    model->resetData(std::move(data));

    table.show();
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