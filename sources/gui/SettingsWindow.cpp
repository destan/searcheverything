#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->listWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void SettingsWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{

    if (!current)
        current = previous;

    ui->stackedWidget->setCurrentIndex(ui->listWidget->row(current));

}
