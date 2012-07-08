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
