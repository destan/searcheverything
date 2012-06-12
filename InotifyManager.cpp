#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
//#include <linux/inotify.h>
#include <sys/inotify.h>
#include <QDebug>

#include "InotifyManager.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int InotifyManager::s_fd;

InotifyManager::InotifyManager(QObject *parent) :
    QObject(parent)
{
}

void InotifyManager::initNotify()
{
    /*creating the INOTIFY instance*/
    s_fd = inotify_init();

    /*checking for error*/
    if ( s_fd < 0 ) {
        perror( "inotify_init" );
    }
}

int InotifyManager::addToWatch(char path[])
{
//    qDebug() << "ADDING PATH TO WATCH " << path;
    /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
    int wd = inotify_add_watch( s_fd, path, IN_CREATE | IN_DELETE | IN_MODIFY );
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
        int i=0;

        /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/
        qDebug() << "READING...";
        length = read( s_fd, buffer, EVENT_BUF_LEN );
        qDebug() << "READ...";

        /*checking for error*/
        if ( length < 0 ) {
            qDebug( "read ERROR" );
        }

        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

        qDebug() << "Le event " << QString::number(event->mask);

        if ( event->len ) {
            if ( event->mask & IN_CREATE ) {
                if ( event->mask & IN_ISDIR ) {
                    qDebug( "New directory %s created.\n", event->name );
                }
                else {
                    qDebug( "New file %s created.\n", event->name );
                }
            }
            else if ( event->mask & IN_DELETE ) {
                if ( event->mask & IN_ISDIR ) {
                    qDebug( "Directory %s deleted.\n", event->name );
                }
                else {
                    qDebug( "File %s deleted.\n", event->name );
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }
    /*removing the “/tmp” directory from the watch list.*/
//    inotify_rm_watch( s_fd, wd );

    /*closing the INOTIFY instance*/
    close( s_fd );
    printf( "BITTI... ");

}
