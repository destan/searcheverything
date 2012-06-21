#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QMainWindow>

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
        Q_OBJECT

    public:
        static SearchWindow* getInstance(){
            static SearchWindow* instance = new SearchWindow();
            return instance;
        }

        static void showIt(){
            getInstance()->show();
        }

        static void updateResults(QStringList resultList);

        ~SearchWindow();

    protected:
        void changeEvent(QEvent *e);

    private slots:
        void on_lineEditSearch_textEdited(const QString &searchKey);
        void triggerSearch();

    private:
        explicit SearchWindow(QWidget *parent = 0);// Hide to ensure singleton pattern
        SearchWindow(const SearchWindow&);// Hide to ensure singleton pattern
        void operator =(const SearchWindow& ); // Hide and don't implement to ensure singleton pattern

        static QString searchKey;

        Ui::SearchWindow *ui;
};

#endif // SEARCHWINDOW_H
