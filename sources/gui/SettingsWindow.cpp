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
#include <QDebug>

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "SettingsManager.h"
#include "core/FileSystemIndexer.h"

QStringList SettingsWindow::selectedDirectories;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->listWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

    /* Load selected directories from settings */
    QStringList preSelectedDirectories = SettingsManager::getSelectedDirectories();

    //FIXME: garbage collection? try to set this as parent
    fsModel = new SelectableFileSystemModel(preSelectedDirectories);
    ui->treeViewFoldersToBeIndexed->setModel(fsModel);

    /* Set home as the default selected path */
    ui->treeViewFoldersToBeIndexed->setCurrentIndex( fsModel->index(QDir::homePath()) );

    /* Hiding unused columns of file system tree */
    ui->treeViewFoldersToBeIndexed->hideColumn(1);//Size
    ui->treeViewFoldersToBeIndexed->hideColumn(2);//Type
    ui->treeViewFoldersToBeIndexed->hideColumn(3);//Date modified

    ui->treeViewFoldersToBeIndexed->setAnimated(true);
    ui->treeViewFoldersToBeIndexed->resizeColumnToContents(0);


    /* Setting first page with first list item programmatically
     * in case of forgatten wrong state on the .ui file
     */
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

    /* Hide reindexing warning initially */
    ui->widgetWarning->hide();

    /* Set values of settings*/
    ui->checkBoxOnlyFiles->setChecked(SettingsManager::get("onlyFiles").toBool());
    ui->checkBoxStartup->setChecked(SettingsManager::get("startAtStartup").toBool());
    ui->lineEditInotifyLimit->setText(SettingsManager::getWatchLimit());

    connect(fsModel, SIGNAL(selectedDirectoriesChanged(QStringList)), this, SLOT(handleSelectedDirectoriesChanged(QStringList)));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::changeEvent(QEvent *e){
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void SettingsWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous){
    if (!current){
        current = previous;
    }
    ui->stackedWidget->setCurrentIndex(ui->listWidget->row(current));
}

void SettingsWindow::on_checkBoxStartup_toggled(bool checked){
    SettingsManager::set("startAtStartup", checked);
}

void SettingsWindow::on_checkBoxOnlyFiles_toggled(bool checked){
    SettingsManager::set("onlyFiles", checked);
}

void SettingsWindow::handleSelectedDirectoriesChanged(QStringList givenSelectedDictionaries){
    QStringList existingList = SettingsManager::getSelectedDirectories();

    /* Sorting of selected directories list is only done here */
    existingList.sort();
    givenSelectedDictionaries.sort();

    if(givenSelectedDictionaries == existingList ){
        ui->widgetWarning->hide();
        selectedDirectories.clear();
    }
    else{
        ui->widgetWarning->show();
        selectedDirectories = givenSelectedDictionaries;
    }
}

void SettingsWindow::on_pushButtonReindexNow_clicked(){
    SettingsManager::set("selectedDirectories", selectedDirectories);
    FileSystemIndexer::reindex();

    /* Reload file system list model*/
    fsModel->setSelectedDirectories(SettingsManager::getSelectedDirectories());

    ui->widgetWarning->hide();
    selectedDirectories.clear();
}
