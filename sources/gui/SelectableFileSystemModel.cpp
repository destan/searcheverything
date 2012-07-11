#include "SelectableFileSystemModel.h"

#include <QDebug>

SelectableFileSystemModel::SelectableFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    setReadOnly(true);
    setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    setRootPath(QDir::homePath());
}

SelectableFileSystemModel::SelectableFileSystemModel(QStringList initiallySelectedDirectories, QObject *parent)
{
    setReadOnly(true);
    setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    setRootPath(QDir::homePath());
    selectedDirectories = initiallySelectedDirectories;
}

void SelectableFileSystemModel::setSelectedDirectories(QStringList list) {
    selectedDirectories = list;
    emit layoutChanged();
    emit selectedDirectoriesChanged(selectedDirectories);
}

QVariant SelectableFileSystemModel::data(const QModelIndex& index, int role) const {
    if (index.isValid() && index.column() == 0 && role == Qt::CheckStateRole) {

        if( selectedDirectories.contains(filePath(index)) ){
            return Qt::Checked;
        }

        foreach (QString selectedPath, selectedDirectories) {
            /* Check if the item is a subdirectory of a selected directory */
            if(filePath(index).contains(QRegExp("^" + selectedPath))){
                return Qt::Checked;
            }
        }

        foreach (QString selectedPath, selectedDirectories) {
            /* Check if the item has a subdirectory that is selected */
            if(selectedPath.contains(filePath(index))){
                return Qt::PartiallyChecked;
            }
        }

        return Qt::Unchecked;
    }
    return QFileSystemModel::data(index, role);
}

bool SelectableFileSystemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == Qt::Checked) {
            //Item is checked so add to the list
            selectedDirectories.append(filePath(index));
        }
        else if(value.toInt() == Qt::Unchecked) {
            //Item is unchecked so remove from the list
            selectedDirectories.removeAll(filePath(index));
        }
        //ignore partially checked items

        emit dataChanged(index, index);
        emit layoutChanged();
        emit selectedDirectoriesChanged(selectedDirectories);

        return true;
    }
    return QFileSystemModel::setData(index, value, role);
}

Qt::ItemFlags SelectableFileSystemModel::flags(const QModelIndex& index) const {
    if (index.column() == 0) { // make only the first column checkable
        return QFileSystemModel::flags(index) | Qt::ItemIsUserCheckable ;
    }
    return QFileSystemModel::flags(index);
}
