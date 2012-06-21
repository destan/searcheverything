#include "SearchWindow.h"
#include "ui_SearchWindow.h"

#include "DatabaseManager.h"
#include <QDebug>

SearchWindow::SearchWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
}

SearchWindow::~SearchWindow()
{
    delete ui;
}

void SearchWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void SearchWindow::on_lineEditSearch_textEdited(const QString &searchKey)
{
    getInstance()->ui->plainTextEdit->clear();

    if(!searchKey.isEmpty() && searchKey.length() > 1){
        //FIXME: implement timeout
        DatabaseManager::search(searchKey.toStdString().c_str());
    }
}

void SearchWindow::updateResults(QStringList resultList)
{
    foreach (QString line, resultList) {
        getInstance()->ui->plainTextEdit->appendPlainText(line);
    }
}
