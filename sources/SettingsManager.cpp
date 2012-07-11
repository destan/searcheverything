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
#include <QApplication>
#include <QFile>
#include <QDesktopServices>
#include <QDebug>

#include "SettingsManager.h"
#include "core/DatabaseManager.h"
#include "core/InotifyManager.h"

QString SettingsManager::databaseFileName;
QStringList SettingsManager::definedSettings;
QVariantMap SettingsManager::settingsContainer;
bool SettingsManager::isLoading = true;//true as default because settings start loading at the constructor

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {
    /* Setup definedSettings to validate
       Whenever a new settings should be added,
     the key first need to be added to this list.
    */
    definedSettings << "startAtStartup"             /* bool */
                    << "indexingDoneBefore"         /* bool */
                    << "onlyFiles"                  /* bool */
                    << "selectedDirectories";       /* QStringList */

    loadSettings();
}

const char *SettingsManager::getDatabaseFileName()
{
    return databaseFileName.toStdString().c_str();
}

QVariant SettingsManager::get(QString key)
{
    if( validateSettingKey(key) ){
        return settingsContainer.value(key);
    }
    return QVariant();
}

void SettingsManager::set(QString key, QVariant value)
{
    if( validateSettingKey(key) ){
        settingsContainer.insert(key, value);

        /* Special settings to handle immediately */
        if( key == "startAtStartup" ){
            updateStartupOption( value.toBool() );
        }

        if( !isLoading ){
            saveSettings();
        }
    }
}

bool SettingsManager::acquireApplicationLock()
{
    static QString lockFileName = QString(databaseFileName).append("Lock");
    QFile lockFile( lockFileName );
    if(lockFile.exists()){
        return false;
    }

    if(!lockFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning("@ApplicationManager::acquireApplicationLock: Cannot create lock file");
        return false;
    }

    qDebug() << "@ApplicationManager::acquireApplicationLock: Lock file created at " << lockFileName;
    lockFile.close();
    return true;
}

bool SettingsManager::releaseApplicationLock()
{
    static QString lockFileName = QString(databaseFileName).append("Lock");

    if(!QFile::remove( lockFileName )){
        qCritical("@ApplicationManager::releaseApplicationLock: Cannot remove lock file");
        return false;
    }
    qDebug("@ApplicationManager::releaseApplicationLock: Removed lock file");
    return true;
}

//PUBLIC SLOT
void SettingsManager::quitApplication()
{
    InotifyManager::stopWatching();
    DatabaseManager::closeDb();
    saveSettings();
    releaseApplicationLock();
}

//PRIVATE METHODS
void SettingsManager::loadSettings()
{
    //Settings info
    QApplication::setOrganizationName(ORG_NAME);
    QApplication::setOrganizationDomain(ORG_DOMAIN);
    QApplication::setApplicationName(APP_NAME);

    QSettings qSettings;

    /* Must be set after settings info set */
    databaseFileName = QDesktopServices::storageLocation(QDesktopServices::DataLocation).append(QDir::separator()).append("database");
    qDebug() << "@SettingsManager::loadSettings: SearchEverything using" << databaseFileName << "as database";

    /* Ensure the path exists */
    QDir().mkpath( QDesktopServices::storageLocation(QDesktopServices::DataLocation) );

    //Application settings
    set("startAtStartup", qSettings.value(APPLICATION_START_AT_STARTUP, false ) );
    set("indexingDoneBefore", qSettings.value(APPLICATION_INDEXING_DONE_BEFORE, false ) );
    set("onlyFiles", qSettings.value(APPLICATION_ONLY_FILES, false ) );
    set("selectedDirectories", qSettings.value(APPLICATION_SELECTED_DIRECTORIES, QStringList() << QDir::homePath() ) );

    isLoading = false;
    qDebug("@SettingsManager::loadSettings: done.");
}

void SettingsManager::saveSettings(){
    //Settings info
    QApplication::setOrganizationName(ORG_NAME);
    QApplication::setOrganizationDomain(ORG_DOMAIN);
    QApplication::setApplicationName(APP_NAME);
    QSettings qSettings;

    //Application settings
    qSettings.setValue(APPLICATION_START_AT_STARTUP, get("startAtStartup") );
    qSettings.setValue(APPLICATION_INDEXING_DONE_BEFORE, get("indexingDoneBefore") );
    qSettings.setValue(APPLICATION_ONLY_FILES, get("onlyFiles") );
    qSettings.setValue(APPLICATION_SELECTED_DIRECTORIES, get("selectedDirectories") );

    qDebug("@SettingsManager::saveSettings: saved.");
}

void SettingsManager::updateStartupOption(bool startup){

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
}

bool SettingsManager::validateSettingKey(QString key)
{
    if(!definedSettings.contains(key)){
        qCritical() << "SettingsManager::loadSettings: Invalid key:" << key;
        throw std::exception();
    }
    return true;
}

