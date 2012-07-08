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

#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QSystemTrayIcon>
#include <QClipboard>
#include <QAction>

class TrayManager : public QObject
{
        Q_OBJECT
    public:
        static TrayManager* getInstance(){
            static TrayManager *instance = new TrayManager();
            return instance;
        }

        static void showTrayMessage(QString title = "",
                                    QString message = "",
                                    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                                    int duration = 3000);

        int init();

    private slots:
        void quitAppSlot();
        void showSearchWindowSlot();
        void showSettingsWindowSlot();

    private:
        explicit TrayManager(QObject *parent = 0);// Hide to ensure singleton pattern
        TrayManager(const TrayManager& ); // Hide and don't implement to ensure singleton pattern
        void operator =(const TrayManager& ); // Hide and don't implement to ensure singleton pattern

        static QSystemTrayIcon *m_pTrayIcon;
        QMenu *trayIconMenu;

        QAction *searchAction;
        QAction *settingsAction;
        QAction *quitAction;
};

#endif // TRAYMANAGER_H
