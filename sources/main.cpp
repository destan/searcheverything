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

    SettingsManager::getInstance();


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

    if( !SettingsManager::get("indexingDoneBefore").toBool() ){
        qDebug("@main: indexing file system...");
        splash.showMessage("indexing file system...", Qt::AlignBottom, Qt::gray);
        FileSystemIndexer::reindex();
    }

    QtConcurrent::run(InotifyManager::startWatching);
    TrayManager::getInstance()->init();

    SearchWindow::showIt();
    splash.finish(SearchWindow::getInstance());

    return a.exec();
}
