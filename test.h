#ifndef TEST_H
#define TEST_H

#include <QDate>

class Test
{
public:
    Test();
    QString getResult() const;
    void setResult(const QString &value);
    QString getSerialNumber() const;
    void setSerialNumber(QString value);
    QString getProductName() const;
    void setProductName(const QString &value);
    QString getBatchNumber() const;
    void setBatchNumber(const QString &value);
    QDateTime getDate() const;
    void setDate(const QDateTime &value);

private:
    QDateTime date;
    QString batchNumber;
    QString productName;
    QString serialNumber;
    QString result;
};

inline bool operator==(const Test &t1, const Test &t2)
{
    return t1.getDate() == t2.getDate();
}

inline uint qHash(const Test &key, uint seed)
{
    return qHash(key.getDate(), seed);
}

#endif // TEST_H
