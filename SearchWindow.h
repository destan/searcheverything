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
        void on_listView_doubleClicked(const QModelIndex &index);
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
