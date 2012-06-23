#include <QApplication>
#include "SearchWindow.h"
#include <time.h>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "Utils.h"
#include <QDebug>
#include <QtConcurrentRun>
#include <QDateTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    QApplication::setQuitOnLastWindowClosed(false);

    clock_t begin = clock();
    QDateTime start = QDateTime::currentDateTimeUtc();

//    FileSystemIndexer::isIndexingDone = true;

    DatabaseManager::initDb();
    InotifyManager::initNotify();

    FileSystemIndexer::indexPath("/home/destan/Desktop", 0);

    QFuture<void> future = QtConcurrent::run(InotifyManager::startWatching);

    clock_t end = clock();

    double elapsed_secs = ((double) (end - begin)) / CLOCKS_PER_SEC;
    QDateTime finish = QDateTime::currentDateTimeUtc();
    qDebug("\n\nIndexing took %f seconds\n", elapsed_secs );

    qint64 t = start.msecsTo(finish) / 1000;
    qDebug() << "real time" << QString::number(t) << t;
    SearchWindow::showIt();

    int retVal = a.exec();
    DatabaseManager::closeDb();
    InotifyManager::stopWatching();
    return retVal;
}
