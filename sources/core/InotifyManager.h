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

#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <QObject>

class InotifyManager : public QObject
{
        Q_OBJECT
    public:

        static InotifyManager* getInstance(){
            static InotifyManager* instance = new InotifyManager();
            return instance;
        }

        static void initNotify();
        static int addToWatch(const char path[]);
        static void startWatching();
        static void stopWatching();

    signals:
        void fileDeleted(std::string fullPath);
        void fileCreated(std::string fullPath);
        void folderDeleted(std::string fullPath, int wd);
        void folderCreated(std::string fullPath);

    private slots:
        void handleFileDeleted(std::string fullPath);
        void handleFileCreated(std::string fullPath);
        void handleFolderDeleted(std::string fullPath, int wd);
        void handleFolderCreated(std::string fullPath);

    private:
        explicit InotifyManager(QObject *parent = 0);// Hide to ensure singleton pattern
        InotifyManager(const InotifyManager&);// Hide to ensure singleton pattern
        void operator =(const InotifyManager& ); // Hide and don't implement to ensure singleton pattern

        static void getWatchlistFromDBAndBind();

        static int s_fd;
        static bool shallStop;

};

#endif // INOTIFYMANAGER_H
