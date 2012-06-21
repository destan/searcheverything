#include <algorithm>
#include <sqlite3.h>
#include <stdio.h>

#include <QString>
#include <QDebug>

#include "DatabaseManager.h"
#include "Utils.h"
#include "SearchWindow.h"
#include "InotifyManager.h"

sqlite3 *DatabaseManager::db;
char* errorMessage;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    // int i;
    // for(i=0; i<argc; i++){
    //   printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    // }
    // printf("\n");
    return 0;
}

static int searchCallback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    QStringList resultList;
    for(i=0; i<argc; i++){
        //        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        resultList.append(argv[i]);
    }

    SearchWindow::updateResults(resultList);

    return 0;
}

//static int getPathByWatchIdCallback(void *NotUsed, int argc, char **argv, char **azColName){
//    int i;
//    QStringList resultList;
//    for(i=0; i<argc; i++){
//        //        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//        resultList.append(argv[i]);
//    }

//    SearchWindow::updateResults(resultList);

//    return 0;
//}

void DatabaseManager::addToIndex(char *p_fileName, char *p_path, std::string fullPath)
{
    char *zErrMsg = 0;
    int rc;

    std::string fileName(p_fileName);
    Utils::replace(fileName, "'", "''");

    std::string path(p_path);
    Utils::replace(path, "'", "''");

    Utils::replace(fullPath, "'", "''");

    int queryLength = 71 + fileName.length() + path.length() + fullPath.length();

    char query[queryLength];
    //    query = (char*) malloc( sizeof(char) * queryLength );

    int result = snprintf(query, sizeof(query)-1, "INSERT INTO fs_index(file_name, path, full_path) VALUES('%s', '%s', '%s');", fileName.c_str(), path.c_str(), fullPath.c_str());
    //    qDebug(query);
    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

    //    free (query);

    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        printf("Query: %s\n", query);
        sqlite3_free(zErrMsg);
    }
}

void DatabaseManager::addToWatchList(int watchId, const char *p_path)
{
    char *zErrMsg = 0;
    int rc;

    std::string path(p_path);
    Utils::replace(path, "'", "''");

    //FIXME
    std::string q = "INSERT INTO fs_folders(watch_id, full_path) VALUES(" + QString::number(watchId).toStdString() +", '" + path + "');";

    //    char query[queryLength];

    //    int result = snprintf(query, sizeof(query)-1, , watchId, fullPath.c_str());
    rc = sqlite3_exec(db, q.c_str(), callback, 0, &zErrMsg);

    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        printf("Query: %s\n", q.c_str());
        sqlite3_free(zErrMsg);
    }
}

void DatabaseManager::initDb()
{
    // Open Database
    int rc = sqlite3_open("database", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    //Create databses
    sqlite3_exec(db, "CREATE VIRTUAL TABLE fs_index USING fts4(file_name TEXT, path TEXT, full_path TEXT);", NULL, NULL, &errorMessage);

    sqlite3_exec(db, "CREATE TABLE fs_folders (watch_id INTEGER, full_path TEXT);", NULL, NULL, &errorMessage);

    if( rc ){
        fprintf(stderr, "Can't create database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
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


void DatabaseManager::search(std::string fullPath)
{
    char *zErrMsg = 0;
    int rc;

    Utils::replace(fullPath, "'", "''");
    char query[ fullPath.length() + 57 ];
    snprintf(query, sizeof(query)-1, "SELECT full_path FROM fs_index WHERE file_name MATCH '%s';", fullPath.c_str());

    rc = sqlite3_exec(db, query, searchCallback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}


std::string DatabaseManager::getPathByWatchId(int watchId)
{
    char query[50];
    int length = sprintf(query, "SELECT * FROM fs_folders where watch_id=%d", watchId);
    sqlite3_stmt * stmt;

    sqlite3_prepare_v2(db, query, length, & stmt, NULL);
    int s = sqlite3_step (stmt);

    qDebug() << "@DatabaseManager::getPathByWatchId: s:" << QString::number(s) ;

    if (s == SQLITE_ROW) {
        const unsigned char * text = sqlite3_column_text (stmt, 1);
        return reinterpret_cast<const char*>(text);
    }
    return "NO PATH";
}

void DatabaseManager::bindToAllIndexedFolders()
{
    sqlite3_stmt * stmt;
    const char *sql = "SELECT distinct full_path FROM fs_folders";
    sqlite3_prepare_v2(db, sql, strlen (sql) + 1, & stmt, NULL);
    int s = sqlite3_step (stmt);

    while (s == SQLITE_ROW) {
        const unsigned char * text = sqlite3_column_text (stmt, 0);
        qDebug("adding to watch: %s\n", text);
        InotifyManager::addToWatch(reinterpret_cast<const char*>(text));

        s = sqlite3_step (stmt);
    }
}
