#ifndef CONVERTER_H
#define CONVERTER_H

#include "component.h"
#include <QObject>
#include <QStringList>
#include <QList>
#include <QProgressDialog>

class Converter : public QObject
{
    Q_OBJECT
public:
    Converter(QObject *parent = 0);
    void loadData(QStringList fileNames);
    void convertAndSave(QString fileName);
private:
    QStringList openingErrors;
    QStringList parsingErrors;
    QHash<QString, Component*> components;
    QProgressDialog *progressDialog;
    QStringList files;
};

#endif // CONVERTER_H
