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


#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <QDebug>
#include <QDir>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"
#include "SettingsManager.h"

int FileSystemIndexer::totalDirs = 0;
int FileSystemIndexer::totalFiles = 0;
bool FileSystemIndexer::isIndexingDoneBefore = false;

void addToWatch(const char* path){
    int watchDescriptor = InotifyManager::addToWatch(path);
    DatabaseManager::addToWatchList(watchDescriptor, path);
}

void FileSystemIndexer::indexPath(const char* path, int level)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))){
        return;
    }

    // indexing the 'path' itself
    addToWatch(path);

    if (!(entry = readdir(dir))){
        return;
    }

    do {// traversing path's children
        if (entry->d_type == DT_DIR) {//is a directory
            char childPath[3000];//FIXME: make dynamic
            int len = snprintf(childPath, sizeof(childPath)-1, "%s/%s", path, entry->d_name);

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }

            addToWatch(childPath);

            ++totalDirs;
            FileSystemIndexer::indexPath(childPath, level + 1);
        }
        else if(entry->d_type == DT_REG){//is a file
            ++totalFiles;
            DatabaseManager::addToIndex(entry->d_name, path, std::string(path).append("/").append(entry->d_name));
        }
    } while ( ( entry = readdir(dir) ) );
    closedir(dir);//being a good boy
}

void FileSystemIndexer::reindex(){
    qDebug("@SearchWindow::reIndex: indexing file system...");
    InotifyManager::stopWatching();
    DatabaseManager::closeDb();
    QFile::remove( SettingsManager::getDatabaseFileName() );

    DatabaseManager::initDb();
    InotifyManager::initNotify();

    DatabaseManager::clear();

    foreach (QString selectedDirectory, SettingsManager::getSelectedDirectories()) {
        FileSystemIndexer::indexPath( selectedDirectory.toStdString().c_str() , 0);
    }

    SettingsManager::set("indexingDoneBefore", true);
}
