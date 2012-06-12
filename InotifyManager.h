#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <QObject>

class InotifyManager : public QObject
{
        Q_OBJECT
    public:
        explicit InotifyManager(QObject *parent = 0);

        static void initNotify();
        static int addToWatch(char path[]);
        static void startWatching();

    private:
        static int s_fd;

};

#endif // INOTIFYMANAGER_H
