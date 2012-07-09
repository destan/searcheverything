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

#include <QFileSystemModel>

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "SelectableFileSystemModel.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->listWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

    SelectableFileSystemModel *fsModel = new SelectableFileSystemModel;

    ui->treeViewFoldersToBeIndexed->setModel(fsModel);

    /* Set home as the default selected path */
    ui->treeViewFoldersToBeIndexed->setCurrentIndex( fsModel->index(QDir::homePath()) );
    ui->treeViewFoldersToBeIndexed->hideColumn(1);//Size
    ui->treeViewFoldersToBeIndexed->hideColumn(2);//Type
    ui->treeViewFoldersToBeIndexed->hideColumn(3);//Date modified

    ui->treeViewFoldersToBeIndexed->setAnimated(true);
    ui->treeViewFoldersToBeIndexed->resizeColumnToContents(0);
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
