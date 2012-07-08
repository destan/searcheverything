#include <QApplication>
#include "gui/SearchWindow.h"
#include <time.h>

#include "core/FileSystemIndexer.h"
#include "core/DatabaseManager.h"
#include "core/InotifyManager.h"
#include "Utils.h"
#include "SettingsManager.h"
#include "TrayManager.h"

#include <QDebug>
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTextCodec>

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


    if(QApplication::arguments().size() > 1){//first argument is the app name
        if(QApplication::arguments().contains(QString("--force"))){
            SettingsManager::releaseApplicationLock();
        }
        else{
            qDebug("Wrong arguments");
            exit(-1);
        }
    }

    if( !SettingsManager::acquireApplicationLock() ){
        splash.showMessage("SearchEverything is already running", Qt::AlignBottom, Qt::gray);
        QMessageBox::critical(0, QObject::trUtf8("SearchEverything is already running!"), QObject::trUtf8("You are already searching like hell!"), QMessageBox::Ok, QMessageBox::Ok);
        return 13;
    }

    //Makin' the universe utf-8, as it should have been at the first place...
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

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
