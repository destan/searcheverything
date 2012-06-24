#include <QApplication>
#include "SearchWindow.h"
#include <time.h>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "Utils.h"
#include "SettingsManager.h"

#include <QDebug>
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    a.connect(&a, SIGNAL(aboutToQuit()), SettingsManager::getInstance(), SLOT(quitApplication()));

    SettingsManager::loadSettings();
    DatabaseManager::initDb();
    InotifyManager::initNotify();

    if( !SettingsManager::isIndexingDoneBefore() ){
        qDebug("@main: indexing file system...");
        FileSystemIndexer::indexPath( QDir::homePath().toStdString().c_str() , 0);//FIXME: hardcode
        SettingsManager::indexingDone();
    }

    QtConcurrent::run(InotifyManager::startWatching);

    SearchWindow::showIt();

    return a.exec();
}
