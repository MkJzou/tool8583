#ifndef FIELDCONFIGDELEGATE_H
#define FIELDCONFIGDELEGATE_H

#include <QStyledItemDelegate>


class FieldConfigDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FieldConfigDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // FIELDCONFIGDELEGATE_H
