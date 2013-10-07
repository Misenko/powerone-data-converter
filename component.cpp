#include "component.h"

Component::Component()
{
}
QString Component::getName() const
{
    return name;
}

void Component::setName(const QString &value)
{
    name = value;
}
QString Component::getLowLimit() const
{
    return lowLimit;
}

void Component::setLowLimit(const QString &value)
{
    lowLimit = value;
}
QString Component::getHighLimit() const
{
    return highLimit;
}

void Component::setHighLimit(const QString &value)
{
    highLimit = value;
}

void Component::addMeasurement(Test *test, QString measurement)
{
    measurements.insert(test, measurement);
}

QString Component::getMeasurement(Test* test){
    return measurements.value(test);
}

bool Component::hasMeasurement(Test* test){
    return measurements.contains(test);
}

bool Component::isEmpty(){
    return lowLimit.isEmpty() && highLimit.isEmpty() && measurements.empty();
}

QList<Test *> Component::getTests()
{
    return measurements.keys();
}
