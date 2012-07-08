#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
        Q_OBJECT

    public:
        explicit SettingsWindow(QWidget *parent = 0);
        ~SettingsWindow();

    protected:
        void changeEvent(QEvent *e);

    private slots:
        void changePage(QListWidgetItem *current, QListWidgetItem *previous);

    private:
        Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
