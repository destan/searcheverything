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
        explicit SearchWindow(QWidget *parent = 0);
        ~SearchWindow();
        
    protected:
        void changeEvent(QEvent *e);
        
    private:
        Ui::SearchWindow *ui;
};

#endif // SEARCHWINDOW_H
