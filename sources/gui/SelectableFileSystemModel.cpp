#include "SelectableFileSystemModel.h"

#include <QDebug>

QStringList SelectableFileSystemModel::selectedDirectories;

SelectableFileSystemModel::SelectableFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    setReadOnly(true);
    setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    setRootPath(QDir::homePath());
}

QVariant SelectableFileSystemModel::data(const QModelIndex& index, int role) const {
    if (index.isValid() && index.column() == 0 && role == Qt::CheckStateRole) {

        if( selectedDirectories.contains(filePath(index)) ){
            return Qt::Checked;
        }

        foreach (QString selectedPath, selectedDirectories) {
            if(filePath(index).contains(QRegExp("^" + selectedPath))){
                return Qt::Checked;
            }
        }

        if(index.column() == 1){
            return QVariant();
        }

        return Qt::Unchecked;
    }
    return QFileSystemModel::data(index, role);
}

bool SelectableFileSystemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && index.column() == 0 && role == Qt::CheckStateRole) {
        if (value.toInt() == Qt::Checked) {

            //CHECKED
            selectedDirectories.append(filePath(index));

        } else {

            //UNCHECKED
            selectedDirectories.removeAll(filePath(index));

        }
        emit dataChanged(index, index);
        emit layoutChanged();

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
