#include <QApplication>
#include "SearchWindow.h"
#include <time.h>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "Utils.h"
#include "SettingsManager.h"
#include "TrayManager.h"

#include <QDebug>
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    a.connect(&a, SIGNAL(aboutToQuit()), SettingsManager::getInstance(), SLOT(quitApplication()));

    SettingsManager::loadSettings();

    if( !SettingsManager::acquireApplicationLock() ){
        QMessageBox::critical(0, QObject::trUtf8("SearchEverything is already running!"), QObject::trUtf8("You are already searching like hell!"), QMessageBox::Ok, QMessageBox::Ok);
        return 13;
    }

    TrayManager::getInstance()->init();
    DatabaseManager::initDb();
    InotifyManager::initNotify();

    if( !SettingsManager::isIndexingDoneBefore() ){
        qDebug("@main: indexing file system...");
        FileSystemIndexer::indexPath( QDir::homePath().toStdString().c_str() , 0);//FIXME: hardcode
        SettingsManager::setIndexingDone(true);
    }

    QtConcurrent::run(InotifyManager::startWatching);

    SearchWindow::showIt();

    return a.exec();
}
