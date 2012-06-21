#include <QApplication>
#include "SearchWindow.h"
#include <time.h>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "Utils.h"
#include <QDebug>
#include <QtConcurrentRun>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    QApplication::setQuitOnLastWindowClosed(false);

    clock_t begin = clock();

    FileSystemIndexer::isIndexingDone = true;

    DatabaseManager::initDb();
    InotifyManager::initNotify();

//    FileSystemIndexer::indexPath("/home/destan", 0);

    QFuture<void> future = QtConcurrent::run(InotifyManager::startWatching);

    clock_t end = clock();

    double elapsed_secs = ((double) (end - begin)) / CLOCKS_PER_SEC;
    qDebug("\n\nIndexing took %f seconds\n", elapsed_secs );

    SearchWindow::showIt();

    int retVal = a.exec();
    DatabaseManager::closeDb();
    InotifyManager::stopWatching();
    return retVal;
}
