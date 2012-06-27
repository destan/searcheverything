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
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    QPixmap pixmap(":/icons/splash.jpg");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    a.connect(&a, SIGNAL(aboutToQuit()), SettingsManager::getInstance(), SLOT(quitApplication()));

    SettingsManager::loadSettings();

    if( !SettingsManager::acquireApplicationLock() ){
        splash.showMessage("SearchEverything is already running", Qt::AlignBottom, Qt::gray);
        QMessageBox::critical(0, QObject::trUtf8("SearchEverything is already running!"), QObject::trUtf8("You are already searching like hell!"), QMessageBox::Ok, QMessageBox::Ok);
        return 13;
    }

    splash.showMessage("adding folders to watch...", Qt::AlignBottom, Qt::gray);

    DatabaseManager::initDb();
    InotifyManager::initNotify();

    if( !SettingsManager::isIndexingDoneBefore() ){
        qDebug("@main: indexing file system...");
        splash.showMessage("indexing file system...", Qt::AlignBottom, Qt::gray);
        FileSystemIndexer::indexPath( QDir::homePath().toStdString().c_str() , 0);//FIXME: hardcode
        SettingsManager::setIndexingDone(true);
    }

    QtConcurrent::run(InotifyManager::startWatching);
    TrayManager::getInstance()->init();

    SearchWindow::showIt();
    splash.finish(SearchWindow::getInstance());

    return a.exec();
}
