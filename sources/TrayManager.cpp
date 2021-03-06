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


#include "TrayManager.h"
#include "gui/SearchWindow.h"
#include "gui/SettingsWindow.h"

#include <QMenu>
#include <QDebug>
#include <QApplication>

QSystemTrayIcon* TrayManager::m_pTrayIcon;

TrayManager::TrayManager(QObject *parent) : QObject(parent){
    TrayManager::m_pTrayIcon = 0;
}

void TrayManager::showTrayMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int duration)
{
    TrayManager::m_pTrayIcon->showMessage(title, message, icon, duration);
}

int TrayManager::init()
{

    if(TrayManager::m_pTrayIcon == 0){
        //Menu actions
        searchAction = new QAction(tr("&Search"), this);
        settingsAction = new QAction(tr("Settings"), this);
        quitAction = new QAction(tr("&Quit"), this);

        //Menu items
        trayIconMenu = new QMenu();
        trayIconMenu->addAction(searchAction);
        trayIconMenu->addAction(settingsAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        //Signal-slot connections
        connect(searchAction, SIGNAL(triggered()), this, SLOT(showSearchWindowSlot()));
        connect(quitAction, SIGNAL(triggered()), this, SLOT(quitAppSlot()));
        connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsWindowSlot()));

        //Put the system tray icon
        const QIcon icon = QIcon(":/icons/trayIcon.png");
        m_pTrayIcon = new QSystemTrayIcon(icon);

        m_pTrayIcon->setContextMenu(trayIconMenu);
        m_pTrayIcon->setToolTip(tr("Searching like hell!"));

        m_pTrayIcon->show();
        qDebug("@TrayManager::init: Tray icon created.");

        if(!m_pTrayIcon->supportsMessages()){
            qWarning("@TrayManager::init: This system does NOT support tray messages.");
        }

        return 0;
    }
    else
    {
        qWarning("Tray icon has been already created, check the code");
        return 31;
    }
}

void TrayManager::quitAppSlot()
{
    QApplication::quit();
}

void TrayManager::showSearchWindowSlot()
{
    SearchWindow::showIt();
}

void TrayManager::showSettingsWindowSlot()
{
    SettingsWindow *s = new SettingsWindow();
    s->show();
}
