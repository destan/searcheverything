#include <QApplication>
#include "SearchWindow.h"
#include <time.h>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "Utils.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    QApplication::setQuitOnLastWindowClosed(false);

    clock_t begin = clock();
    //----------------------------------------------
    //    DatabaseManager::initDb();
    //    FileSystemIndexer::indexPath("/", 0);
    //    DatabaseManager::closeDb();
    //----------------------------------------------
    //    DatabaseManager::search("cevirgec");
    //----------------------------------------------
    InotifyManager::initNotify();
    DatabaseManager::initDb();
    FileSystemIndexer::indexPath("/home/destan", 0);
    DatabaseManager::closeDb();
    InotifyManager::startWatching();
    //----------------------------------------------

    clock_t end = clock();

    double elapsed_secs = ((double) (end - begin)) / CLOCKS_PER_SEC;
    qDebug("\n\nIndexing took %f seconds\n", elapsed_secs );

    //    std::string text("/home/destan/.config/sublime-text-2/Packages/PHP/$_FILES[''].sublime-snippet");

    //     Utils::replace(text, "'", "''");
    //    qDebug() << text.c_str();


    //    SearchWindow w;
    //    w.show();

    //    return a.exec();
    return 0;
}
