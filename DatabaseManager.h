#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <sqlite3.h>

class DatabaseManager
{
    public:
        static void addToIndex(char *p_fileName, std::string fullPath);
        static void removeFromIndex(char* p_fileName, std::string fullPath);
        static void search(std::string fullPath);
        static void initDb();
        static void closeDb();

    private:
//        static void callback(void *NotUsed, int argc, char **argv, char **azColName);
        static sqlite3 *db;
};

#endif // DATABASEMANAGER_H


/*
./db database "SELECT count(*) FROM fs_index"
./db database "SELECT count(*) FROM fs_index WHERE file_name MATCH 'cevirgec'"

./db database "SELECT file_name FROM fs_index WHERE file_name MATCH 'cevirgec'"
./db database "SELECT full_path FROM fs_index WHERE file_name MATCH 'cevirgec'"
 */
