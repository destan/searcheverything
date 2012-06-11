#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <QDebug>

#include "FileSystemIndexer.h"
#include "DatabaseManager.h"

int FileSystemIndexer::totalDirs = 0;
int FileSystemIndexer::totalFiles = 0;

void FileSystemIndexer::indexPath(char* name, int level)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
    if (!(entry = readdir(dir)))
        return;

    do {
        if (entry->d_type == DT_DIR) {
            char path[3000];//FIXME make dynamic
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            //            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
//            qDebug("%*s[%s]\n", level*2, "", entry->d_name);
            ++totalDirs;
            FileSystemIndexer::indexPath(path, level + 1);
        }
        else if(entry->d_type == DT_REG){
            ++totalFiles;
//            qDebug("%*s- %s\n", level*2, "", entry->d_name);
            DatabaseManager::writeIndexToDb(entry->d_name, std::string(name));
        }
    } while ((entry = readdir(dir)));
    closedir(dir);
}
