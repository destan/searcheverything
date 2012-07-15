/*
    AUTHOR:
    Destan Sarpkaya - 2012
    destan@dorukdestan.com

    LICENSE:
    SearchEverything is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SearchEverything is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SearchEverything.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/SearchWindow.h"
#include "ui_SearchWindow.h"
#include "Utils.h"

#include "core/DatabaseManager.h"
#include "core/InotifyManager.h"
#include "core/FileSystemIndexer.h"
#include "SettingsManager.h"

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
#include <QLabel>
#include <QDir>
#include <QtConcurrentRun>
#include <QFutureWatcher>

static QTimer *timer;
QString SearchWindow::searchKey;

QStandardItemModel *SearchWindow::pModel = new QStandardItemModel();
QAction *SearchWindow::showInFolderAction = new QAction(trUtf8("Show in folder"), 0);
QAction *SearchWindow::openAction = new QAction(trUtf8("Open"), 0);
QAction *SearchWindow::showTrashAction = new QAction(trUtf8("Show trash"), 0);

bool SearchWindow::isAlive = false;

SearchWindow::SearchWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    pModel->setHorizontalHeaderLabels(QStringList() << trUtf8("Name") << trUtf8("Path"));
    ui->treeView->setModel(pModel);

    ui->statusBar->showMessage(trUtf8("Enter file or directory name"));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(triggerSearch()));

    connect(this, SIGNAL( customContextMenuRequested(const QPoint &) ), this, SLOT( showContextMenu(const QPoint &) ) );
    connect(showInFolderAction, SIGNAL( triggered() ), this, SLOT( handleShowInFolderAction() ) );
    connect(openAction, SIGNAL( triggered() ), this, SLOT( handleOpenAction() ) );
    connect(showTrashAction, SIGNAL( triggered() ), this, SLOT( handleShowTrashActionAction() ) );

    ui->checkBoxOnlyFiles->setChecked( SettingsManager::get("onlyFiles").toBool() );
    ui->labelReindexingWait->hide();

    Utils::centerWindow(this);
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
//    pModel->clear();
    pModel->removeRows(0, pModel->rowCount());
    event->accept();
}

void SearchWindow::on_lineEditSearch_textEdited(const QString &searchKey)
{
    ui->statusBar->showMessage(trUtf8("Seaching..."));
//    pModel->clear();
    pModel->removeRows(0, pModel->rowCount());


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
    SearchWindow::searchKey = searchKey.trimmed();

    if( SettingsManager::get("onlyFiles").toBool() ){
        DatabaseManager::searchFiles(SearchWindow::searchKey.toStdString().c_str());
    }
    else{
        DatabaseManager::searchFiles(SearchWindow::searchKey.toStdString().c_str());
        DatabaseManager::searchFolders(SearchWindow::searchKey.toStdString().c_str());
    }

    static QString foundMessage = " item(s) found";
    ui->statusBar->showMessage( QString::number( ui->treeView->model()->rowCount() ).append(foundMessage) );
}

void SearchWindow::showContextMenu(const QPoint &position)
{
    QMenu contextMenu(trUtf8("Context menu"), this);

    QMap<int, QVariant> resultMap = ui->treeView->model()->itemData( ui->treeView->currentIndex() );
    QString fullPath = resultMap.value(Qt::UserRole + 1).toString();

    QFileInfo selectedItem(fullPath);

    if( !selectedItem.exists() ){
        contextMenu.addAction(showTrashAction);
    }
    else if( selectedItem.isDir() ){
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
    QMap<int, QVariant> resultMap = ui->treeView->model()->itemData( ui->treeView->currentIndex() );
    QString fullPath = resultMap.value(Qt::UserRole + 1).toString();

    QString path = QString( Utils::getPath( fullPath.toStdString() ).c_str() );

    QDesktopServices::openUrl( QUrl("file:///" + path) );
}

void SearchWindow::handleOpenAction()
{
    QMap<int, QVariant> resultMap = ui->treeView->model()->itemData( ui->treeView->currentIndex() );
    QString fullPath = resultMap.value(Qt::UserRole + 1).toString();

    if(QFileInfo(fullPath).exists()){
        QDesktopServices::openUrl( QUrl("file:///" + fullPath) );
    }else{
        QDesktopServices::openUrl( QUrl("trash:///") );
    }
}

void SearchWindow::handleShowTrashActionAction()
{
    QDesktopServices::openUrl( QUrl("trash:///") );
}

void SearchWindow::quitApplication()
{
    QApplication::quit();
}

void SearchWindow::updateResults(QStringList resultList)
{
    static QFileIconProvider iconProvider;
    static QIcon trashIcon(":/icons/trash.png");

    foreach (QString line, resultList) {
        QStandardItem *item = new QStandardItem(line.split("/", QString::SkipEmptyParts).last());
        item->setData(line);
        QIcon icon = iconProvider.icon( QFileInfo(line) );

        if(icon.isNull()){
            item->setIcon( trashIcon );
        }
        else{
            item->setIcon( icon );
        }

        int rowCount = pModel->rowCount();
        pModel->setItem(rowCount, item);
        pModel->setItem(rowCount, 1, new QStandardItem(line));//append a column to the current row

        /* Allow user interaction during search and partial loading of results*/
        QApplication::processEvents();
    }
    getInstance()->ui->treeView->resizeColumnToContents(0);
}

void SearchWindow::on_checkBoxOnlyFiles_toggled(bool checked)
{
    SettingsManager::set("onlyFiles", checked);
}

void SearchWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    handleOpenAction();
}

void reindex(){
    FileSystemIndexer::reindex();
}

void SearchWindow::on_actionReindex_triggered()
{
    int result = QMessageBox::question(0,"Are you sure",
                                       "Do you want to reindex your home folder? This may take several minutes depending on your home folder's size.",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    ui->treeView->hide();
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
    ui->treeView->show();
    ui->labelReindexingWait->hide();
    ui->centralWidget->setEnabled(true);
    ui->menuBar->setEnabled(true);
    ui->statusBar->showMessage(trUtf8("Enter file or directory name"));
}
