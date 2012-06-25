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

    private:
        explicit TrayManager(QObject *parent = 0);// Hide to ensure singleton pattern
        TrayManager(const TrayManager& ); // Hide and don't implement to ensure singleton pattern
        void operator =(const TrayManager& ); // Hide and don't implement to ensure singleton pattern

        static QSystemTrayIcon *m_pTrayIcon;
        QMenu *trayIconMenu;

        QAction *searchAction;
        QAction *quitAction;
};

#endif // TRAYMANAGER_H
