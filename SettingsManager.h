#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QSettings>

static const char APPLICATION_START_AT_STARTUP[] = "application/startAtStartup";
static const char APPLICATION_INDEXING_DONE_BEFORE[] = "application/indexingDoneBefore";
static const char APPLICATION_ONLY_FILES[] = "application/onlyFiles";

static const char HOTKEY_SEARCH[] = "hotkey/search";

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

        static void loadSettings();
        static void updateStartupOption(bool);

        static QString getHotKeySearch();
        static const char *getDatabaseFileName();
        static bool isIndexingDoneBefore();
        static bool isOnlyFiles();

        static void setHotKeySearch(QString hotKeySearch);
        static void setStartAtStartup(bool startAtStartup);
        static void setOnlyFiles(bool isOnlyFiles);
        static void setIndexingDone(bool isDone);

        static bool acquireApplicationLock();
        static bool releaseApplicationLock();

    public slots:
        void quitApplication();

    private:
        static void saveSettings();
        explicit SettingsManager(QObject *parent = 0);// Hide to ensure singleton pattern
        SettingsManager(const SettingsManager&);// Hide to ensure singleton pattern
        void operator =(const SettingsManager& ); // Hide and don't implement to ensure singleton pattern


        //Application
        static bool startAtStartup;
        static bool indexingDoneBefore;
        static bool onlyFiles;
        static QString databaseFileName;

        //Hotkey
        static QString hotKeySearch;
};


#endif // SETTINGSMANAGER_H
