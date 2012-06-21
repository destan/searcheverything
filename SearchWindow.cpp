#include "SearchWindow.h"
#include "ui_SearchWindow.h"

#include "DatabaseManager.h"
#include <QDebug>
#include <QTimer>

static QTimer *timer;
QString SearchWindow::searchKey;

SearchWindow::SearchWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::SearchWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(triggerSearch()));
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
    SearchWindow::searchKey = searchKey;

    if(!searchKey.isEmpty() && searchKey.length() > 1){
        qDebug("reset timer");
        timer->start(700);
    }
}

void SearchWindow::triggerSearch()
{
    qDebug("timeout");
    DatabaseManager::search(SearchWindow::searchKey.toStdString().c_str());
}

void SearchWindow::updateResults(QStringList resultList)
{
    foreach (QString line, resultList) {
        getInstance()->ui->plainTextEdit->appendPlainText(line);
    }
}
