/*
    AUTHOR:
    Destan Sarpkaya - 2012
    destan@dorukdestan.com

    LICENSE:
    SearchEverything is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cevirgec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cevirgec.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDesktopServices>
#include "SettingsManager.h"
#include "DatabaseManager.h"
#include "InotifyManager.h"

//Application
bool SettingsManager::startAtStartup;
bool SettingsManager::indexingDoneBefore;
bool SettingsManager::onlyFiles;
QString SettingsManager::databaseFileName;

//Hotkey
QString SettingsManager::hotKeySearch;

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {}

void SettingsManager::loadSettings()
{

    //Settings info
    QApplication::setOrganizationName(ORG_NAME);
    QApplication::setOrganizationDomain(ORG_DOMAIN);
    QApplication::setApplicationName(APP_NAME);

    QSettings qSettings;

    //Must be set after settings info set
    databaseFileName = QDesktopServices::storageLocation(QDesktopServices::DataLocation).append(QDir::separator()).append("database");
    qDebug() << "SearchEverything using" << databaseFileName << "as database";

    //ensure the path exists
    QDir().mkpath( QDesktopServices::storageLocation(QDesktopServices::DataLocation) );

    //Application
    startAtStartup = qSettings.value(APPLICATION_START_AT_STARTUP, false).toBool();
    indexingDoneBefore = qSettings.value(APPLICATION_INDEXING_DONE_BEFORE, false).toBool();
    onlyFiles = qSettings.value(APPLICATION_ONLY_FILES, false).toBool();

    //Hotkey
    hotKeySearch = qSettings.value(HOTKEY_SEARCH, "CTRL+SHIFT+S").toString();

    qDebug("Settings loaded.");
}

void SettingsManager::saveSettings(){
    //Settings info
    QApplication::setOrganizationName(ORG_NAME);
    QApplication::setOrganizationDomain(ORG_DOMAIN);
    QApplication::setApplicationName(APP_NAME);
    QSettings qSettings;

    //Application
    qSettings.setValue(APPLICATION_START_AT_STARTUP, startAtStartup);
    qSettings.setValue(APPLICATION_INDEXING_DONE_BEFORE, indexingDoneBefore);
    qSettings.setValue(APPLICATION_ONLY_FILES, onlyFiles);

    //Hotkey
    qSettings.setValue(HOTKEY_SEARCH, hotKeySearch);

    qDebug("Settings saved.");
}

void SettingsManager::updateStartupOption(bool startup){
    SettingsManager::startAtStartup = startup;

    if(startup){

        //create autostart if not exists
        if(!QDir(QDir::home().absolutePath().append("/.config/autostart/SearchEverything.desktop")).exists()){
            QDir autostartFolder;
            autostartFolder.mkdir((QDir::home().absolutePath().append("/.config/autostart")));
        }

        if(!QFile::copy("/usr/share/applications/SearchEverything.desktop", QDir::home().absolutePath().append("/.config/autostart/SearchEverything.desktop"))){
            qWarning("@SettingsManager::updateStartupOption: Unable to copy shortcut file");
        }
    }else{
        if(!QFile::remove(QDir::home().absolutePath().append("/.config/autostart/SearchEverything.desktop"))) {
            qWarning("@SettingsManager::updateStartupOption: Unable to remove shortcut file");
        }
    }

    saveSettings();
}


bool SettingsManager::isIndexingDoneBefore()
{
    return indexingDoneBefore;
}

bool SettingsManager::isOnlyFiles()
{
    return onlyFiles;
}


void SettingsManager::setIndexingDone(bool isDone)
{
    indexingDoneBefore = isDone;
    saveSettings();
}

void SettingsManager::quitApplication()
{
    InotifyManager::stopWatching();
    DatabaseManager::closeDb();
    saveSettings();
}


const char *SettingsManager::getDatabaseFileName()
{
    return databaseFileName.toStdString().c_str();
}


void SettingsManager::setOnlyFiles(bool isOnlyFiles)
{
    onlyFiles = isOnlyFiles;
    saveSettings();
}
