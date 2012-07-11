#ifndef SELECTABLEFILESYSTEMMODEL_H
#define SELECTABLEFILESYSTEMMODEL_H

#include <QFileSystemModel>

class SelectableFileSystemModel : public QFileSystemModel
{
        Q_OBJECT
    public:
        explicit SelectableFileSystemModel(QObject *parent = 0);
        explicit SelectableFileSystemModel(QStringList initiallySelectedDirectories, QObject *parent = 0);

        void setSelectedDirectories(QStringList list);

        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
        void selectedDirectoriesChanged(QStringList);

    private:
        QStringList selectedDirectories;

        void showWarningIfNecessary();
};

#endif // SELECTABLEFILESYSTEMMODEL_H
