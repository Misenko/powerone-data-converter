#ifndef COMPONENT_H
#define COMPONENT_H

#include "test.h"
#include <QString>
#include <QHash>

class Component {
public:
    Component();
    QString getName() const;
    void setName(const QString &value);

    QString getLowLimit() const;
    void setLowLimit(const QString &value);

    QString getHighLimit() const;
    void setHighLimit(const QString &value);
    void addMeasurement(Test* test, QString measurement);
    QString getMeasurement(Test* test);
    bool hasMeasurement(Test* test);
    bool isEmpty();
    QList<Test*> getTests();

private:
    QString name;
    QString lowLimit;
    QString highLimit;
    QHash<Test*, QString> measurements;
};

#endif // COMPONENT_H
