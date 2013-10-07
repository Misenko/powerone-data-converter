#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "converter.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->addFilesButton, SIGNAL(clicked()), this, SLOT(addFiles()));
    connect(ui->removeFilesButton, SIGNAL(clicked()), this, SLOT(removeFiles()));
    connect(ui->clearFilesButton, SIGNAL(clicked()), this, SLOT(clearFiles()));
    connect(ui->fileNamesListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(fileSelected()));
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(convertFiles()));
    connect(ui->outputFileToolButton, SIGNAL(clicked()), this, SLOT(selectOutputFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addFiles(){
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            QDir::homePath(),
                            tr("Web pages (*.html *.xhtml *.htm)"));
    if(!files.empty()){
        ui->fileNamesListWidget->addItems(files);
    }
}

void MainWindow::removeFiles(){
    qDeleteAll(ui->fileNamesListWidget->selectedItems());
    ui->removeFilesButton->setEnabled(false);
}

void MainWindow::clearFiles(){
    ui->fileNamesListWidget->clear();
    ui->removeFilesButton->setEnabled(false);
}

void MainWindow::fileSelected(){
    if(ui->fileNamesListWidget->selectedItems().count() != 0){

        ui->removeFilesButton->setEnabled(true);
    }
    else
        ui->removeFilesButton->setEnabled(false);
}

void MainWindow::selectOutputFile(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select output file"),
                               QDir::homePath(),
                               tr("CSV file(*.csv)"));
    QRegExp re("\\..*$");
    if(!fileName.isEmpty() && !fileName.contains(re)){
        fileName.append(".csv");
    }
    ui->outputFileLineEdit->setText(fileName);
}

void MainWindow::convertFiles(){
    if(ui->fileNamesListWidget->count() == 0){
        QMessageBox::critical(this, tr("No input files"), tr("There are no input files to convert."), QMessageBox::Close, QMessageBox::Close);
        return;
    }

    if(ui->outputFileLineEdit->text().isEmpty()){
        QMessageBox::critical(this, tr("No output file"), tr("The output file was not chosen."), QMessageBox::Close, QMessageBox::Close);
        return;
    }

    QStringList fileNames;
    foreach(QListWidgetItem *item, ui->fileNamesListWidget->findItems("*", Qt::MatchWildcard)){
        fileNames.append(item->text());
    }

    Converter *converter = new Converter(this);
    converter->loadData(fileNames);
    converter->convertAndSave(ui->outputFileLineEdit->text());
}
