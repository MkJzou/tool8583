#include "fieldconfigdelegate.h"

#include <QLineEdit>
#include <QRegExp>
#include <QComboBox>
#include <QDebug>


FieldConfigDelegate::FieldConfigDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *FieldConfigDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
//    qDebug() << "createEditor";
    if(0 <= index.column() && index.column() <= 4)
    {
        switch (index.column())
        {
        case 0:
        {
            QLineEdit* widget = new QLineEdit(parent);
            return widget;
        }
        case 1:
        {
            QLineEdit* widget = new QLineEdit(parent);
            return widget;
        }
        case 2:
        {
            QLineEdit* widget = new QLineEdit(parent);
            QRegExp reg("^(a|n|s|b|an|ns|ans)[1-9][0-9]*");
            widget->setValidator(new QRegExpValidator(reg, parent));
            return widget;
        }
        case 3:
        {
            QComboBox* widget = new QComboBox(parent);
            widget->addItems(QStringList() << "LLVAR" << "LLLVAR" << "");
            return widget;
        }
        case 4:
        {
            QComboBox* widget = new QComboBox(parent);
            widget->addItems(QStringList() << "ASCII" << "BCD" << "BINARY" << "BYTE");
            return widget;
        }
        }

    }
//    qDebug() << "null";
    return NULL;
}

void FieldConfigDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
//    qDebug() << "setEditorData";
    QString value = index.model()->data(index, Qt::EditRole).toString();
//    qDebug() << value;
    if (3 <= index.column() && index.column() <= 4)
    {
        QComboBox* box = static_cast<QComboBox*>(editor);
        box->setCurrentText(value);
    }
    else if (0 <= index.column() && index.column() <= 2)
    {
        QLineEdit* edit = static_cast<QLineEdit*>(editor);
        edit->setText(value);
    }
}

void FieldConfigDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
//    qDebug() << "setModelData";
    if (3 <= index.column() && index.column() <= 4)
    {
        QComboBox* box = static_cast<QComboBox*>(editor);
        model->setData(index, box->currentText(), Qt::EditRole);
    }
    else if (0 <= index.column() && index.column() <= 2)
    {
        QLineEdit* edit = static_cast<QLineEdit*>(editor);
        model->setData(index, edit->text(), Qt::EditRole);
    }
}

void FieldConfigDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
//    qDebug() << "updateEditorGeometry";
    editor->setGeometry(option.rect);
}
