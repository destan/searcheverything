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
#include "Utils.h"
#include "SettingsManager.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int InotifyManager::s_fd;
bool InotifyManager::shallStop = false;

InotifyManager::InotifyManager(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(fileCreated(std::string)), this, SLOT(handleFileCreated(std::string)));
    connect(this, SIGNAL(fileDeleted(std::string)), this, SLOT(handleFileDeleted(std::string)));
    connect(this, SIGNAL(folderCreated(std::string)), this, SLOT(handleFolderCreated(std::string)));
    connect(this, SIGNAL(folderDeleted(std::string, int)), this, SLOT(handleFolderDeleted(std::string, int)));
}

void InotifyManager::initNotify()
{
    /*creating the INOTIFY instance*/
    s_fd = inotify_init();

    /*checking for error*/
    if ( s_fd < 0 ) {
        perror( "inotify_init" );
        qCritical("@InotifyManager::initNotify: Cannot initialize an inotify instance.");
    }

    if( SettingsManager::isIndexingDoneBefore() ){
        qDebug("@InotifyManager::initNotify: indexing done before, adding indexed folders to watch list...");
        getWatchlistFromDBAndBind();
    }
}

int InotifyManager::addToWatch(const char path[])
{
    /* Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
    int wd = inotify_add_watch( s_fd, path, IN_CREATE | IN_DELETE | IN_MOVE  );
    if(wd < 0){
        qCritical() <<" ERROR ADDING WATCH FOR " << path << " with error:" << strerror(errno);
    }
    return wd;
}

/**
 * Need to be executed in a seperate thread.
 * @brief InotifyManager::startWatching
 */
void InotifyManager::startWatching()
{
    int length;
    char buffer[EVENT_BUF_LEN];

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
            qCritical( "@InotifyManager::startWatching: read ERROR" );
        }

        while ( i < length ) {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

            if ( event->len ) {
                if ( (event->mask & IN_CREATE) || (event->mask & IN_MOVED_TO) ) {// CREATE or MOVED TO EVENT
                    if ( event->mask & IN_ISDIR ) {
                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "New directory %s created.\n" << path.c_str() ;
                        emit(getInstance()->folderCreated(path));
                    }
                    else {
                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "New file %s created.\n" << path.c_str() ;
                        emit(getInstance()->fileCreated(path));
                    }
                }
                else if ( (event->mask & IN_DELETE) || (event->mask & IN_MOVED_FROM) ) { // DELETE or MOVED FROM EVENT
                    if ( event->mask & IN_ISDIR ) {
                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug( ) << "Directory %s deleted or moved from.\n" << path.c_str();
                        emit(getInstance()->folderDeleted(path, event->wd));
                    }
                    else {
                        std::string path = DatabaseManager::getPathByWatchId(event->wd);
                        path.append("/").append(event->name);
                        qDebug(  ) << "File %s deleted.\n" << path.c_str();
                        emit(getInstance()->fileDeleted(path));
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

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
    DatabaseManager::removeFromIndex(fullPath);
}

void InotifyManager::handleFileCreated(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFileCreated" << fullPath.c_str();
    DatabaseManager::addToIndex(Utils::getFileName(fullPath).c_str(), Utils::getPath(fullPath).c_str(), fullPath);
}

void InotifyManager::handleFolderDeleted(std::string fullPath, int wd)
{
    qDebug() << "InotifyManager::handleFolderDeleted" << fullPath.c_str();
    inotify_rm_watch( s_fd, wd );
    DatabaseManager::removeFromWatchList(fullPath);
}

void InotifyManager::handleFolderCreated(std::string fullPath)
{
    qDebug() << "InotifyManager::handleFolderCreated" << fullPath.c_str();

    int watchDescriptor = InotifyManager::addToWatch(const_cast<const char*>(fullPath.c_str()));
    DatabaseManager::addToWatchList(watchDescriptor, fullPath.c_str());
}
