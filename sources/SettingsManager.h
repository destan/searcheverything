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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QSettings>

/* Setting Keys to be used internally by SettingsManager, not used by other classes */
static const char APPLICATION_START_AT_STARTUP[] = "application/startAtStartup";
static const char APPLICATION_INDEXING_DONE_BEFORE[] = "application/indexingDoneBefore";
static const char APPLICATION_ONLY_FILES[] = "application/onlyFiles";
static const char APPLICATION_SELECTED_DIRECTORIES[] = "application/selectedDirectories";

/* Settings Information */
static const char ORG_NAME[] = "DorukDestan";
static const char ORG_DOMAIN[] = "searcheverything.dorukdestan.com";
static const char APP_NAME[] = "SearchEverything";

class SettingsManager : public QObject
{
        Q_OBJECT
    public:

        static SettingsManager* getInstance(){
            static SettingsManager* instance = new SettingsManager();
            return instance;
        }

        static const char *getDatabaseFileName();

        static QVariant get(QString key);
        static void set(QString key, QVariant value);

        static bool acquireApplicationLock();
        static bool releaseApplicationLock();

        static QStringList getSelectedDirectories();

    public slots:
        void quitApplication();

    private:
        explicit SettingsManager(QObject *parent = 0);// Hide to ensure singleton pattern
        SettingsManager(const SettingsManager&);// Hide to ensure singleton pattern
        void operator =(const SettingsManager& ); // Hide and don't implement to ensure singleton pattern

        static void loadSettings();
        static void saveSettings();
        static void updateStartupOption(bool startup);
        static bool validateSettingKey(QString key);

        /* Key are camelcase
           Never return this map to another class
            as it may be go out of date while the class uses it
        */
        static QVariantMap settingsContainer;

        /* This list is used to validate the settings key
          when a setting value requested via get()
        */
        static QStringList definedSettings;

        static QString databaseFileName;
        static bool isLoading;
};


#endif // SETTINGSMANAGER_H
