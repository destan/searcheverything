#include <algorithm>
#include <sqlite3.h>
#include <stdio.h>

#include <QString>
#include <QDebug>

#include "DatabaseManager.h"
#include "Utils.h"
#include "SearchWindow.h"

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

void DatabaseManager::addToIndex(char *p_fileName, std::string fullPath)
{
    char *zErrMsg = 0;
    int rc;

    //    static std::string oldVal = "'";
    //    static std::string newVal = "'";

    //    std::string fileName(p_fileName);

    /*
        FIXME: make with pure std, without qt, use Utils::replace
    */
    //    QString fileName(p_fileName);
    //    fileName.replace("'", "''");

    //    QString fullPath(fp.c_str());
    //    fullPath.replace("'", "''");

    std::string fileName(p_fileName);
    Utils::replace(fileName, "'", "''");
    Utils::replace(fullPath, "'", "''");

    //----------------------

    int queryLength = 61 + fileName.length() + fullPath.length();

    char query[queryLength];
    //    query = (char*) malloc( sizeof(char) * queryLength );

    int result = snprintf(query, sizeof(query)-1, "INSERT INTO fs_index(file_name, full_path) VALUES('%s', '%s');", fileName.c_str(), fullPath.c_str());
    //    qDebug(query);
    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

    //    free (query);

    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        printf("Query: %s\n", query);
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

    //Create databse
    //"CREATE VIRTUAL TABLE fs_index.db USING fts4(file_name TEXT, full_path TEXT);"
    sqlite3_exec(db, "CREATE VIRTUAL TABLE fs_index USING fts4(file_name TEXT, full_path TEXT);", NULL, NULL, &errorMessage);

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
    //    sqlite3_free(errorMessage);
    sqlite3_close(db);
}

//int callback(void *NotUsed, int argc, char **argv, char **azColName){
//    // int i;
//    // for(i=0; i<argc; i++){
//    //   printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    // }
//    // printf("\n");
//    return 0;
//}


void DatabaseManager::search(std::string fullPath)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    Utils::replace(fullPath, "'", "''");
    char query[ fullPath.length() + 57 ];
    snprintf(query, sizeof(query)-1, "SELECT full_path FROM fs_index WHERE file_name MATCH '%s';", fullPath.c_str());

    rc = sqlite3_open("database", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    rc = sqlite3_exec(db, query, searchCallback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
}
