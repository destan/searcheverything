#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
//#include <linux/inotify.h>
#include <sys/inotify.h>
#include <QDebug>

#include "InotifyManager.h"
#include "DatabaseManager.h"
#include "FileSystemIndexer.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int InotifyManager::s_fd;
bool InotifyManager::shallStop = false;

InotifyManager::InotifyManager(QObject *parent) :
    QObject(parent)
{

    connect(this, SIGNAL(fileCreated(std::string)), this, SLOT(handleFileCreated(std::string)));
    connect(this, SIGNAL(fileDeleted(std::string)), this, SLOT(handleFileDeleted(std::string)));
    connect(this, SIGNAL(folderCreated(std::string)), this, SLOT(handleFolderCreated(std::string)));
    connect(this, SIGNAL(folderDeleted(std::string)), this, SLOT(handleFolderDeleted(std::string)));
}

void InotifyManager::initNotify()
{
    /*creating the INOTIFY instance*/
    s_fd = inotify_init();

    /*checking for error*/
    if ( s_fd < 0 ) {
        perror( "inotify_init" );
    }

    if( FileSystemIndexer::isIndexingDone ){
        getWatchlistFromDBAndBind();
    }
}

int InotifyManager::addToWatch(const char path[])
{
    //    qDebug() << "ADDING PATH TO WATCH " << path;

    /* Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
    int wd = inotify_add_watch( s_fd, path, IN_CREATE | IN_DELETE | IN_MOVE  );
    if(wd < 0){
        qCritical() <<" ERROR ADDING WATCH FOR " << path << " with error:" << strerror(errno);
    }
    return wd;
}

void InotifyManager::startWatching()
{
    int length;
    int wd;
    char buffer[EVENT_BUF_LEN];

    /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
    forever {

        if(shallStop){
            break;
        }

        int i=0;

        /* This read blocks until the change event occurs*/
        length = read( s_fd, buffer, EVENT_BUF_LEN );

        if(shallStop){
            break;
        }

        /*checking for error*/
        if ( length < 0 ) {
            qDebug( "read ERROR" );
        }

        while ( i < length ) {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

            qDebug() << "mask" << event->mask << event->name << "i" << i << "length" << length;

            if ( event->len ) {
                if ( event->mask & IN_CREATE ) {
                    if ( event->mask & IN_ISDIR ) {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "New directory %s created.\n" << path.c_str() ;
                        emit(getInstance()->folderCreated(path));
                    }
                    else {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();


                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "New file %s created.\n" << path.c_str() ;
                        emit(getInstance()->fileCreated(path));
                    }
                }
                else if ( event->mask & IN_DELETE ) {
                    if ( event->mask & IN_ISDIR ) {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "Directory %s deleted.\n" << path.c_str();
                        emit(getInstance()->folderDeleted(path));
                    }
                    else {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug(  ) << "File %s deleted.\n" << path.c_str();
                        emit(getInstance()->fileDeleted(path));
                    }
                }
                else if ( event->mask & IN_MOVED_FROM ) {
                    if ( event->mask & IN_ISDIR ) {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug(  ) << "Directory %s moved from.\n" << path.c_str();
                        //                    emit(getInstance()->folderMovedFrom(path));
                    }
                    else {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) <<  "File %s moved from.\n" << path.c_str();
                        //                    emit(getInstance()->fileMovedFrom(path));
                    }
                }
                else if ( event->mask & IN_MOVED_TO ) {
                    if ( event->mask & IN_ISDIR ) {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug(  ) << "Directory %s moved to.\n" << path.c_str();
                        //                    emit(getInstance()->folderMovedTo(path));
                    }
                    else {
//                        qDebug() << event->wd;
//                        qDebug() << DatabaseManager::getPathByWatchId(event->wd).c_str();

                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug(  ) << "File %s moved to.\n" << path.c_str();
                        //                    emit(getInstance()->fileMovedTo(path));
                    }
                }
            }
            qDebug() << "adding" << EVENT_SIZE + event->len;
            i += EVENT_SIZE + event->len;
        }
    }

    /*removing the “/tmp” directory from the watch list.*/
    //    inotify_rm_watch( s_fd, wd );

    /*closing the INOTIFY instance*/
    close( s_fd );
}

void InotifyManager::stopWatching()
{
    shallStop = true;
}

void InotifyManager::getWatchlistFromDBAndBind()
{
    DatabaseManager::bindToAllIndexedFolders();
}


void InotifyManager::handleFileDeleted(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFileDeleted" << fullPath.c_str();
}

void InotifyManager::handleFileCreated(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFileCreated" << fullPath.c_str();
}

void InotifyManager::handleFolderDeleted(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFolderDeleted" << fullPath.c_str();
}

void InotifyManager::handleFolderCreated(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFolderCreated" << fullPath.c_str();
}
