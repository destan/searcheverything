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

#ifndef FILESYSTEMINDEXER_H
#define FILESYSTEMINDEXER_H

#include <string>

class FileSystemIndexer
{
    public:
        static bool isIndexingDoneBefore;
        static void reindex();

    private:
        static void indexPath(const char *name, int level);
        static int totalDirs;
        static int totalFiles;
};

#endif // FILESYSTEMINDEXER_H
