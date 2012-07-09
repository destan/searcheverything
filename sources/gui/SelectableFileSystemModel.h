#ifndef SELECTABLEFILESYSTEMMODEL_H
#define SELECTABLEFILESYSTEMMODEL_H

#include <QFileSystemModel>

class SelectableFileSystemModel : public QFileSystemModel
{
        Q_OBJECT
    public:
        explicit SelectableFileSystemModel(QObject *parent = 0);

        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        Qt::ItemFlags flags(const QModelIndex &index) const;

    private:
        static QStringList selectedDirectories;

};

#endif // SELECTABLEFILESYSTEMMODEL_H
