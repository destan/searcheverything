#include "SearchWindow.h"
#include "ui_SearchWindow.h"
#include "Utils.h"

#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "FileSystemIndexer.h"

#include <QCloseEvent>
#include <QDebug>
#include <QTimer>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QIcon>
#include <QFileIconProvider>
#include <QMessageBox>
#include <InotifyManager.h>
#include <QLabel>
#include <QDir>
#include <QtConcurrentRun>
#include <QFutureWatcher>

static QTimer *timer;
QString SearchWindow::searchKey;

QStandardItemModel *SearchWindow::pModel = new QStandardItemModel();
QAction *SearchWindow::showInFolderAction = new QAction(trUtf8("Show in folder"), 0);
QAction *SearchWindow::openAction = new QAction(trUtf8("Open"), 0);
bool SearchWindow::isAlive = false;

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
    connect(openAction, SIGNAL( triggered() ), this, SLOT( handleOpenAction() ) );

    ui->checkBoxOnlyFiles->setChecked( SettingsManager::isOnlyFiles() );
    ui->labelReindexingWait->hide();
    isAlive = true;
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

void SearchWindow::closeEvent(QCloseEvent *event)
{
    isAlive = false;
    pModel->clear();
    event->accept();
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

    SearchWindow::searchKey = searchKey.trimmed();

    if( SettingsManager::isOnlyFiles() ){
        DatabaseManager::searchFiles(SearchWindow::searchKey.toStdString().c_str());
    }
    else{
        DatabaseManager::searchFiles(SearchWindow::searchKey.toStdString().c_str());
        DatabaseManager::searchFolders(SearchWindow::searchKey.toStdString().c_str());
    }

    static QString foundMessage = " item(s) found";
    ui->statusBar->showMessage( QString::number( ui->listView->model()->rowCount() ).append(foundMessage) );
}

void SearchWindow::showContextMenu(const QPoint &position)
{
    QMenu contextMenu(trUtf8("Context menu"), this);

    QMap<int, QVariant> resultMap = ui->listView->model()->itemData( ui->listView->currentIndex() );
    QString fullPath = resultMap.value(Qt::DisplayRole).toString();

    QFileInfo selectedItem(fullPath);
    if( selectedItem.isDir() ){
        contextMenu.addAction(openAction);
    }
    else{
        contextMenu.addAction(openAction);
        contextMenu.addAction(showInFolderAction);
    }
    contextMenu.exec(this->mapToGlobal(position));
}

void SearchWindow::handleShowInFolderAction()
{
    QMap<int, QVariant> resultMap = ui->listView->model()->itemData( ui->listView->currentIndex() );
    QString fullPath = resultMap.value(Qt::DisplayRole).toString();

    QString path = QString( Utils::getPath( fullPath.toStdString() ).c_str() );

    QDesktopServices::openUrl( QUrl("file:///" + path) );
}

void SearchWindow::handleOpenAction()
{
    QMap<int, QVariant> resultMap = ui->listView->model()->itemData( ui->listView->currentIndex() );
    QString fullPath = resultMap.value(Qt::DisplayRole).toString();

    QDesktopServices::openUrl( QUrl("file:///" + fullPath) );
}

void SearchWindow::quitApplication()
{
    QApplication::quit();
}

void SearchWindow::updateResults(QStringList resultList)
{
    static QFileIconProvider iconProvider;

    foreach (QString line, resultList) {
        QStandardItem *item = new QStandardItem(line);
        item->setIcon( iconProvider.icon( QFileInfo(line) ) );
        pModel->appendRow(item);
    }
}

void SearchWindow::on_checkBoxOnlyFiles_toggled(bool checked)
{
    SettingsManager::setOnlyFiles(checked);
}

void SearchWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    handleOpenAction();
}

void reindex(){
    qDebug("@SearchWindow::reIndex: indexing file system...");
    InotifyManager::stopWatching();
    DatabaseManager::closeDb();
    QFile::remove( SettingsManager::getDatabaseFileName() );

    DatabaseManager::initDb();
    InotifyManager::initNotify();

    FileSystemIndexer::indexPath( QDir::homePath().toStdString().c_str() , 0);//FIXME: hardcode
    SettingsManager::setIndexingDone(true);
}

void SearchWindow::on_actionReindex_triggered()
{
    int result = QMessageBox::question(0,"Are you sure",
                                       "Do you want to reindex your home folder? This may take several minutes depending on your home folder's size.",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    ui->listView->hide();
    ui->labelReindexingWait->show();
    ui->centralWidget->setEnabled(false);
    ui->menuBar->setEnabled(false);
    ui->statusBar->showMessage(trUtf8("reindexing..."));

    // Instantiate the objects and connect to the finished signal.
    static QFutureWatcher<void> watcher;
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinishedReindexing()));

    // Start the computation.
    QFuture<void> future = QtConcurrent::run(reindex);
    watcher.setFuture(future);
}

void SearchWindow::handleFinishedReindexing(){
    ui->listView->show();
    ui->labelReindexingWait->hide();
    ui->centralWidget->setEnabled(true);
    ui->menuBar->setEnabled(true);
    ui->statusBar->showMessage(trUtf8("Enter file or directory name"));
}
