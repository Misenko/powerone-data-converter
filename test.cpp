#include "test.h"

Test::Test()
{
}
QString Test::getResult() const
{
    return result;
}

void Test::setResult(const QString &value)
{
    result = value;
}
QString Test::getSerialNumber() const
{
    return serialNumber;
}

void Test::setSerialNumber(QString value)
{
    serialNumber = value;
}
QString Test::getProductName() const
{
    return productName;
}

void Test::setProductName(const QString &value)
{
    productName = value;
}
QString Test::getBatchNumber() const
{
    return batchNumber;
}

void Test::setBatchNumber(const QString &value)
{
    batchNumber = value;
}
QDateTime Test::getDate() const
{
    return date;
}

void Test::setDate(const QDateTime &value)
{
    date = value;
}

inline bool Test::operator<(const Test &t)
{
    return date < t.getDate();
}





