/**
  ********************************************************************************
  * @file           : table_data.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_DATA_H
#define TABLE_DATA_H

#include <QString>
#include <QRgb>
#include <QMetaType>


class TableData {
public:
    enum Type {
        Real,
        Display,
        Align,
        Foreground,
        Background,
        Font,
    };
    enum FontType {
        None = 0,
        Bold = 1,
        Italic = 2,
        Underline = 4,
    };
    enum AlignType {
        Left = 0x01,        // 0000 0001
        Right = 0x02,       // 0000 0010
        HCenter = 0x04,     // 0000 0100
        Top = 0x20,         // 0010 0000
        Bottom = 0x30,      // 0011 0000
        VCenter = 0x40,     // 0100 0000
        Center = HCenter | VCenter,
    };

    explicit TableData(const QString &value = "");
    TableData(const TableData &data);
    TableData(TableData &&data) noexcept;
    virtual ~TableData() = default;

    QVariant data(Type type) const;
    bool setData(Type type, const QVariant &data);
    bool setData(Type type, QVariant &&data);
    void reset();

    QByteArray serializeData(bool compress = false, int compressionLevel = -1) const;
    bool deserializeData(const QByteArray &data, bool decompress = false);

    bool equal(const TableData &other, const QVector<Type> &types) const;
    bool equal(const TableData &other, Type type) const;

    bool operator==(const TableData &other) const;
    bool operator!=(const TableData &other) const;
    TableData &operator=(const TableData &data);
    TableData &operator=(TableData &&data) noexcept;
    bool operator<(const TableData& other) const;

    friend QDebug operator<<(QDebug debug, const TableData &obj);
    friend QDebug operator<<(QDebug debug, const TableData *obj);
    friend QDataStream &operator<<(QDataStream &out, const TableData &obj);
    friend QDataStream &operator>>(QDataStream &in, TableData &obj);
    friend size_t qHash(const TableData& obj);
    friend size_t qHash(const TableData *obj);

private:
    // value
    QString m_value;
    QString m_display;
    quint8 m_align;
    // color
    QRgb m_foreground;
    QRgb m_background;
    // font
    quint16 m_family;
    quint8 m_fontType;  // bold italic underline
    quint8 m_fontSize;
};

Q_DECLARE_METATYPE(TableData)


#endif //TABLE_DATA_H
