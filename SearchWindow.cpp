#include "SearchWindow.h"
#include "ui_SearchWindow.h"
#include "Utils.h"

#include "DatabaseManager.h"
#include "SettingsManager.h"

#include <QDebug>
#include <QTimer>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>

static QTimer *timer;
QString SearchWindow::searchKey;

QStandardItemModel *SearchWindow::pModel = new QStandardItemModel();
QAction *SearchWindow::showInFolderAction = new QAction(trUtf8("Show in folder"), 0);

SearchWindow::SearchWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->listView->setModel(pModel);

    ui->statusBar->showMessage(trUtf8("Enter file or directory name"));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(triggerSearch()));

    connect(this, SIGNAL( customContextMenuRequested(const QPoint &) ), this, SLOT( showContextMenu(const QPoint &) ) );
    connect(showInFolderAction, SIGNAL( triggered() ), this, SLOT( handleShowInFolderAction() ) );
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
    ui->statusBar->showMessage(trUtf8("Seaching..."));
    pModel->clear();
    SearchWindow::searchKey = searchKey;

    if(!searchKey.isEmpty() && searchKey.length() > 1){
        qDebug("reset timer");
        timer->start(700);
    }
    else if(searchKey.length() == 1){
        ui->statusBar->showMessage(trUtf8("At least 2 chracters needed"));
    }
    else{
        ui->statusBar->showMessage(trUtf8("Enter file or directory name"));
    }
}

void SearchWindow::triggerSearch()
{
    qDebug("timeout");

    DatabaseManager::search(SearchWindow::searchKey.toStdString().c_str());

    static QString foundMessage = " item(s) found";
    ui->statusBar->showMessage( QString::number( ui->listView->model()->rowCount() ).append(foundMessage) );
}

void SearchWindow::showContextMenu(const QPoint &position)
{
    QMenu contextMenu(trUtf8("Context menu"), this);
    contextMenu.addAction(showInFolderAction);
    contextMenu.exec(this->mapToGlobal(position));
}

void SearchWindow::handleShowInFolderAction()
{
    QMap<int, QVariant> resultMap = ui->listView->model()->itemData( ui->listView->currentIndex() );
    QString fullPath = resultMap.value(Qt::DisplayRole).toString();

    QString path = QString( Utils::getPath( fullPath.toStdString() ).c_str() );

    QDesktopServices::openUrl( QUrl("file:///" + path) );
}

void SearchWindow::quitApplication()
{
    QApplication::quit();
}

void SearchWindow::updateResults(QStringList resultList)
{
    foreach (QString line, resultList) {
        QStandardItem *item = new QStandardItem(line);
        pModel->appendRow(item);
    }
}
