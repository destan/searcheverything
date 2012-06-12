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

void FileSystemIndexer::indexPath(char* path, int level)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;
    if (!(entry = readdir(dir)))
        return;

    do {
        if (entry->d_type == DT_DIR) {
            char childPath[3000];//FIXME make dynamic
            int len = snprintf(childPath, sizeof(childPath)-1, "%s/%s", path, entry->d_name);


            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            int watchDescriptor = InotifyManager::addToWatch(childPath);
            DatabaseManager::addToWatchList(watchDescriptor, childPath);

            ++totalDirs;
            FileSystemIndexer::indexPath(childPath, level + 1);
        }
        else if(entry->d_type == DT_REG){
            ++totalFiles;

            DatabaseManager::addToIndex(entry->d_name, path, std::string(path).append("/").append(entry->d_name));
        }
    } while ((entry = readdir(dir)));
    closedir(dir);
}
