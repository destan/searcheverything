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


#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <sqlite3.h>

class DatabaseManager
{
    public:
        static void initDb();
        static void closeDb();

        static void addToIndex(const char *p_fileName, const char *p_path, std::string fullPath);
        static void removeFromIndex(std::string fullPath);

        static void addToWatchList(int watchId, const char* p_path);
        static void removeFromWatchList(std::string fullPath);

        static std::string getPathByWatchId(int watchId);
        static void bindToAllIndexedFolders();

        static void searchFiles(std::string fullPath);
        static void searchFolders(std::string fullPath);

    private:
        static sqlite3 *db;
};

#endif // DATABASEMANAGER_H


/*
./db database "SELECT count(*) FROM fs_index"
./db database "SELECT count(*) FROM fs_index WHERE file_name MATCH 'cevirgec'"

./db database "SELECT file_name FROM fs_index WHERE file_name MATCH 'cevirgec'"
./db database "SELECT full_path FROM fs_index WHERE file_name MATCH 'cevirgec'"
 */
