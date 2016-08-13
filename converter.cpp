#include "converter.h"
#include "test.h"

#include <QFile>
#include <QDate>
#include <QRegExp>
#include <QString>
#include <QDebug>
#include <QMessageBox>

Converter::Converter(QObject *parent) :
QObject(parent) {
}

void Converter::loadData(QStringList fileNames) {
    files = fileNames;
    progressDialog = new QProgressDialog("Reading files...", "", 0, fileNames.count() + 1, NULL);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(NULL);
    progressDialog->setMinimumDuration(2000);

    foreach(QString fileName, fileNames) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            openingErrors.append(fileName);
            continue;
        }

        Test *test = new Test();

        //getting test header
        QTextStream in(&file);
        in.setCodec("Windows-1250");
        QString line = in.readLine();
        bool fail = true;
        int field = 0;
        QList<QRegExp*> reList;
        reList.append(new QRegExp("\\d+-(\\d+-\\d+-\\d+-\\d+-\\d+-\\d+)-\\d+", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("Batch Number: <.*>(.*)<", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("UUT Name: <.*>(.*)<", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("Serial Number: <.*>(.*)<", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("UUT Result: <B><.*>(.*)<.*></B>", Qt::CaseInsensitive, QRegExp::RegExp2));
        while (!in.atEnd()) {
            if (reList.at(field)->indexIn(line, 0) != -1) {
                switch (field) {
                    case 0:
                        test->setDate(QDateTime::fromString(reList.at(field)->cap(1), "yyyy-M-d-h-m-s"));
                        break;
                    case 1:
                        test->setBatchNumber(reList.at(field)->cap(1));
                        break;
                    case 2:
                        test->setProductName(reList.at(field)->cap(1));
                        break;
                    case 3:
                        test->setSerialNumber(reList.at(field)->cap(1));
                        break;
                    case 4:
                        test->setResult(reList.at(field)->cap(1));
                        break;
                }

                field++;
            }

            if (field > 4) {
                fail = false;
                break;
            }

            line = in.readLine();
        }

        if (fail) {
            parsingErrors.append(fileName);
            file.close();
            continue;
        }

        //getting test results
        line = in.readLine();
        fail = true;
        bool hasStatus = false;
        bool hasLowLimit = false;
        bool hasHighLimit = false;
        field = 0;
        Component *component = NULL;
        reList.clear();
        reList.append(new QRegExp("<A NAME=.*>(.*)</A>", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("(Test\\s*[0-9]+:.*)", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("Status:.*>(.*)", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("Measurement:.*>(.*)", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("Low:.*>(.*)", Qt::CaseInsensitive, QRegExp::RegExp2));
        reList.append(new QRegExp("High:.*>(.*)", Qt::CaseInsensitive, QRegExp::RegExp2));
        while (!in.atEnd()) {

            //checking name field
            QString name = NULL;
            if ((reList.at(0)->indexIn(line, 0)) != -1) {
                name = reList.at(0)->cap(1);
            } else if ((reList.at(1)->indexIn(line, 0)) != -1) {
                name = reList.at(1)->cap(1);
            }

            if (name != NULL) {
                if (name == "SequenceCall") {
                    line = in.readLine();
                    continue;
                }

                if (component != NULL && !components.contains(component->getName())) {
                    components.insert(component->getName(), component);
                }

                if (components.contains(name)) {
                    component = components.value(name);
                } else {
                    component = new Component();
                    component->setName(name);
                }

                hasStatus = false;
                hasLowLimit = false;
                hasHighLimit = false;
                fail = true;
            }

            //checking status field
            if ((reList.at(2)->indexIn(line, 0)) != -1) {
                if (reList.at(2)->cap(1).contains("Skipped")) {
                    if (component->isEmpty())
                        delete component;
                    component = NULL;
                }

                if (reList.at(2)->cap(1).contains("Done")) {
                    line = in.readLine();
                    continue;
                }

                if (hasStatus)
                    break;

                hasStatus = true;
            }

            //checking measurement field
            if ((reList.at(3)->indexIn(line, 0)) != -1) {
                if (component->hasMeasurement(test))
                    break;

                component->addMeasurement(test, reList.at(3)->cap(1));
            }

            //checking low limit field
            if ((reList.at(4)->indexIn(line, 0)) != -1) {
                if (hasLowLimit)
                    break;

                hasLowLimit = true;
                if (component->getLowLimit().isEmpty())
                    component->setLowLimit(reList.at(4)->cap(1));
                fail = false;
            }

            //checking high limit field
            if ((reList.at(5)->indexIn(line, 0)) != -1) {
                if (hasHighLimit)
                    break;

                hasHighLimit = true;
                if (component->getHighLimit().isEmpty())
                    component->setHighLimit(reList.at(5)->cap(1));
            }

            line = in.readLine();
        }

        if (fail) {
            qDebug() << line;
            parsingErrors.append(fileName);
            file.close();
            continue;
        }

        if (component != NULL && !components.contains(component->getName())) {
            components.insert(component->getName(), component);
        }

        progressDialog->setValue(progressDialog->value() + 1);
        file.close();
    }
}

void Converter::convertAndSave(QString fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical((QWidget*) parent(), tr("Output error"), tr("Can't open the output file"), QMessageBox::Close, QMessageBox::Close);
        progressDialog->cancel();
        return;
    }

    //fill list of all tests
    QList<Test*> tests;

    foreach(Component *component, components.values()) {

        foreach(Test* test, component->getTests()) {
            if (!tests.contains(test))
                tests.append(test);
        }
    }

    qSort(tests.begin(), tests.end(), testLessThan);

    //write header
    QTextStream out(&file);

    out << ",,";

    foreach(Test *test, tests) {
        out << ",Date: " + test->getDate().toString("dd.MM.yyyy hh:mm:ss");
    }
    out << endl << ",,";

    foreach(Test *test, tests) {
        out << ",Batch no.: " + test->getBatchNumber();
    }
    out << endl << ",,";

    foreach(Test *test, tests) {
        out << ",Status: " + test->getResult();
    }
    out << endl << ",,";

    foreach(Test *test, tests) {
        out << ",Product: " + test->getProductName();
    }
    out << endl << "Item,Low limit,High limit";

    foreach(Test *test, tests) {
        out << ",Serial no.: " + test->getSerialNumber();
    }
    out << endl;

    /*QTextStream out(&file);
    out << "Item,Low limit,High limit";

    foreach(Test *test, tests) {
        out << ",\"Date: " + test->getDate().toString("dd.MM.yyyy hh:mm:ss") << endl;
        out << "Batch no.: " + test->getBatchNumber() << endl;
        out << "Status: " + test->getResult() << endl;
        out << "Product: " + test->getProductName() << endl;
        out << "Serial no.: " + test->getSerialNumber() + "\"";
    }

    out << endl;*/

    progressDialog->setValue(0);
    progressDialog->setMaximum(components.count());
    progressDialog->setLabelText("Saving data...");
    progressDialog->setAutoClose(true);
    QList<QString> keys = components.keys();
    qSort(keys.begin(), keys.end(), naturalCompare);

    foreach(QString name, keys) {
        Component *component = components.value(name);
        out << "\"" + component->getName() + "\",";
        out << component->getLowLimit() + ",";
        out << component->getHighLimit();

        foreach(Test *test, tests) {
            if (component->getTests().contains(test)) {
                out << "," + component->getMeasurement(test);
            } else {
                out << ",";
            }
        }

        out << endl;
        progressDialog->setValue(progressDialog->value() + 1);
    }

    file.close();

    if (parsingErrors.count() != 0 || openingErrors.count() != 0) {
        progressDialog->cancel();
        QString message("Next error(s) occured:\n");
        if (openingErrors.count() != 0) {
            message.append("Unable to open file(s): ");

            foreach(QString file, openingErrors) {
                message.append(file + ", ");
            }
            message.chop(2);
            message.append(".");
        }
        if (parsingErrors.count() != 0) {
            if (openingErrors.count() != 0)
                message.append("\n");
            message.append("Invalid format of file(s): ");

            foreach(QString file, parsingErrors) {
                message.append(file + ", ");
            }
            message.chop(2);
            message.append(".");
        }

        QMessageBox::critical((QWidget*) parent(), tr("Error"), message, QMessageBox::Close, QMessageBox::Close);

        openingErrors.append(parsingErrors);
        qSort(openingErrors);
        qSort(files);
        if (openingErrors != files) {
            QMessageBox::information((QWidget*) parent(), tr("Done"), tr("The conversion of remaining files was successfull."), QMessageBox::Ok, QMessageBox::Ok);
        } else {
            file.remove();
        }
    } else {
        QMessageBox::information((QWidget*) parent(), tr("Done"), tr("The conversion was successfull."), QMessageBox::Ok, QMessageBox::Ok);
    }
}
