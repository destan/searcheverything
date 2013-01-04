
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

#include <algorithm>
#include <sqlite3.h>
#include <stdio.h>

#include <QString>
#include <QDebug>
#include <QFileInfo>

#include "DatabaseManager.h"
#include "Utils.h"
#include "gui/SearchWindow.h"
#include "InotifyManager.h"
#include "SettingsManager.h"

sqlite3 *DatabaseManager::db;
char* errorMessage;

static int searchFilesCallback(void *firstArgument, int argc, char **argv, char **azColName) {
    Q_UNUSED(azColName)
    Q_UNUSED(firstArgument)

    QStringList resultList;
    for(int i = 0; i < argc; i++){
        resultList.append( argv[i] );
    }

    SearchWindow::updateResults(resultList);
    return 0;
}

void DatabaseManager::addToIndex(const char *p_fileName, const char *p_path, std::string fullPath) {
    std::string fileName(p_fileName);
    Utils::replace(fileName, "'", "''");

    std::string path(p_path);
    Utils::replace(path, "'", "''");

    Utils::replace(fullPath, "'", "''");

    int queryLength = 71 + fileName.length() + path.length() + fullPath.length();

    char query[queryLength];

    snprintf(query, sizeof(query)-1, "INSERT INTO fs_index(file_name, path, full_path) VALUES('%s', '%s', '%s');", fileName.c_str(), path.c_str(), fullPath.c_str());
    executeQuery(query, 0);
}

void DatabaseManager::removeFromIndex(std::string fullPath) {
    Utils::replace(fullPath, "'", "''");
    std::string q = "DELETE FROM fs_index WHERE full_path='" + fullPath + "';";
    executeQuery(q.c_str(), 0);
}

void DatabaseManager::addToWatchList(int watchId, const char *p_path)
{
    std::string path(p_path);
    Utils::replace(path, "'", "''");

    //FIXME: optimize
    std::string query = "INSERT INTO fs_folders(watch_id, full_path) VALUES(" + QString::number(watchId).toStdString() +", '" + path + "');";
    executeQuery(query.c_str());
}

void DatabaseManager::removeFromWatchList(std::string fullPath)
{
    Utils::replace(fullPath, "'", "''");
    std::string q = "DELETE FROM fs_folders WHERE full_path='" + fullPath + "';";
    executeQuery(q.c_str());
}

void DatabaseManager::initDb()
{
    // Open and create(if neccessary) database
    QFileInfo databaseFile( SettingsManager::getDatabaseFileName() );
    bool databaseCreatedBefore = databaseFile.exists();

    int rc = sqlite3_open(SettingsManager::getDatabaseFileName(), &db);
    if( rc ){
        fprintf(stderr, "@DatabaseManager::initDb: Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return;
        //FIXME: this is fatal, end the program!
    }

    if( databaseCreatedBefore && !checkTableExistence() ){
        databaseCreatedBefore = false;
    }

    if( !SettingsManager::get("indexingDoneBefore").toBool() || !databaseCreatedBefore ){
        //Create databases
        qDebug("@DatabaseManager::initDb: creating DBs");
        sqlite3_exec(db, "CREATE VIRTUAL TABLE fs_index USING fts4(file_name TEXT, path TEXT, full_path TEXT);", NULL, NULL, &errorMessage);
        sqlite3_exec(db, "CREATE TABLE fs_folders (watch_id INTEGER, full_path TEXT);", NULL, NULL, &errorMessage);

        if( rc ){
            fprintf(stderr, "Can't create database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return ;
            //FIXME: this is fatal, end the program!
        }
        SettingsManager::set("indexingDoneBefore", false);
    }

    // BEGIN TRANSACTION
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);

    // Pragma statements
    sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);
    sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);
    sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);
    sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);
}

void DatabaseManager::closeDb()
{
    // Commit transaction and close database
    sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
    sqlite3_free(errorMessage);
    sqlite3_close(db);
}


void DatabaseManager::searchFiles(std::string fullPath) {
    Utils::replace(fullPath, "'", "''");
    char query[ fullPath.length() + 57 ];
    snprintf(query, sizeof(query)-1, "SELECT full_path FROM fs_index WHERE file_name MATCH '%s';", fullPath.c_str());

    executeQuery(query, searchFilesCallback);
}

void DatabaseManager::searchFolders(std::string fullPath) {
    Utils::replace(fullPath, "'", "''");
    char query[ fullPath.length() + 57 ];
    snprintf(query, sizeof(query)-1, "SELECT DISTINCT path FROM fs_index WHERE path MATCH '%s';", fullPath.c_str());

    executeQuery(query, searchFilesCallback);
}

void DatabaseManager::clear() {
    /* Removing the database file and then reinitializing the
      database over again is way more fast than DELETE FROM solution.

       Besides, if I use other solution, after 'empty'ing the tables by DELETE FROM, the database file doesn't
      shrinks. Don't know why.
    */
    closeDb();
    QFile::remove( SettingsManager::getDatabaseFileName() );
    initDb();

    qDebug("DatabaseManager::clear: done");
}

std::string DatabaseManager::getPathByWatchId(int watchId) {
    char query[50];
    int length = sprintf(query, "SELECT * FROM fs_folders where watch_id=%d", watchId);
    sqlite3_stmt * stmt;

    sqlite3_prepare_v2(db, query, length, & stmt, NULL);
    int result = sqlite3_step (stmt);

    if (result == SQLITE_ROW) {
        const unsigned char * text = sqlite3_column_text (stmt, 1);
        sqlite3_finalize(stmt);
        return reinterpret_cast<const char*>(text);
    }
    sqlite3_finalize(stmt);
    return "NO PATH";// FIXME: handle more elegantly
}

void DatabaseManager::bindToAllIndexedFolders() {
    sqlite3_stmt * stmt;
    const char *sql = "SELECT distinct full_path FROM fs_folders";
    sqlite3_prepare_v2(db, sql, strlen (sql) + 1, & stmt, NULL);
    int result = sqlite3_step (stmt);

    while (result == SQLITE_ROW) {
        const unsigned char * text = sqlite3_column_text (stmt, 0);
        InotifyManager::addToWatch(reinterpret_cast<const char*>(text));

        result = sqlite3_step (stmt);
    }
    sqlite3_finalize(stmt);
}

void DatabaseManager::executeQuery(const char *query, int (*callback)(void*,int,char**,char**), void *firstArgumentToCallback) {
    char *zErrMsg = 0;
    int result = sqlite3_exec(db, query, callback, firstArgumentToCallback, &zErrMsg);

    if( result != SQLITE_OK ){
        fprintf(stderr, "DatabaseManager::executeQuery: %s\n", zErrMsg);
        fprintf(stderr, "DatabaseManager::executeQuery: Query: %s\n", query);
        sqlite3_free(zErrMsg);
    }
}

bool DatabaseManager::checkTableExistence() {
    sqlite3_stmt * stmt;
    const char *sql  = "SELECT count(*) FROM fs_folders";
    const char *sql2 = "SELECT count(*) FROM fs_index";
    int count, count2;
    sqlite3_prepare_v2(db, sql, strlen (sql) + 1, & stmt, NULL);
    int result = sqlite3_step (stmt);

    if (result == SQLITE_ROW) {
        count = sqlite3_column_int (stmt, 0);
    }
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, sql2, strlen (sql2) + 1, & stmt, NULL);
    result = sqlite3_step (stmt);

    if (result == SQLITE_ROW) {
        count2 = sqlite3_column_int (stmt, 0);
    }
    sqlite3_finalize(stmt);

    return count > 0 && count2 > 0;
}
