#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <QDebug>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"

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
