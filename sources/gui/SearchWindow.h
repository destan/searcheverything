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

#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
        Q_OBJECT

    public:
        static SearchWindow* getInstance(){

            static SearchWindow* instance;
            if(!isAlive){
                instance = new SearchWindow();
            }
            return instance;
        }

        static void showIt(){
            getInstance()->show();
        }

        static void updateResults(QStringList resultList);

        ~SearchWindow();

    signals:
        void resultsReady();

    protected:
        void changeEvent(QEvent *e);
        void closeEvent(QCloseEvent *e);

    private slots:
        void on_lineEditSearch_textEdited(const QString &searchKey);
        void triggerSearch();
        void showContextMenu(const QPoint &position);
        void handleShowInFolderAction();
        void handleOpenAction();
        void handleShowTrashActionAction();
        void quitApplication();
        void handleFinishedReindexing();

        void on_checkBoxOnlyFiles_toggled(bool checked);
        void on_treeView_doubleClicked(const QModelIndex &index);
        void on_actionReindex_triggered();


    private:
        explicit SearchWindow(QWidget *parent = 0);// Hide to ensure singleton pattern
        SearchWindow(const SearchWindow&);// Hide to ensure singleton pattern
        void operator =(const SearchWindow& ); // Hide and don't implement to ensure singleton pattern

        static QString searchKey;

        static QStandardItemModel *pModel;
        static QAction *showInFolderAction;
        static QAction *openAction;
        static QAction *showTrashAction;
        static bool isAlive;

        Ui::SearchWindow *ui;
};

#endif // SEARCHWINDOW_H
