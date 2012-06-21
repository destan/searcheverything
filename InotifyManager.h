#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <QObject>

class InotifyManager : public QObject
{
        Q_OBJECT
    public:
        explicit InotifyManager(QObject *parent = 0);

        static void initNotify();
        static int addToWatch(const char path[]);
        static void startWatching();
        static void stopWatching();

    private:
        static void getWatchlistFromDBAndBind();

        static int s_fd;
        static bool shallStop;

};

#endif // INOTIFYMANAGER_H
