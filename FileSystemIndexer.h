#ifndef FILESYSTEMINDEXER_H
#define FILESYSTEMINDEXER_H

#include <string>

class FileSystemIndexer
{
    public:
        static void indexPath(char *name, int level);

        static bool isIndexingDone;
    private:
        static int totalDirs;
        static int totalFiles;
};

#endif // FILESYSTEMINDEXER_H
