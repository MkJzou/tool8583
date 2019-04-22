#ifndef FIELDCONFIG_H
#define FIELDCONFIG_H

#include <QWidget>
#include "config.h"


class QTreeView;
class QStandardItemModel;
class QStandardItem;

class FieldConfig : public QWidget
{
    Q_OBJECT
public:
    explicit FieldConfig(QWidget *parent = nullptr);

private:
    static QSize size_;

signals:

public slots:
    void slotTreeMenu(const QPoint &pos);
    void slotInsertItem(const QModelIndex &index);
    void slotAddSubItem(const QModelIndex &index);
    void slotDeleteItem(const QModelIndex &index);

public:
    void show();
    void closeEvent(QCloseEvent *event) override;

private:
    void uiInit();

    void setTreeItems(const config::Field &field, QStandardItem *parentItem);
    config::FIELDS getConfigFields(QStandardItem *pRootItem);

private:
    QStandardItemModel *pModel_;
    QTreeView *pTreeView_;

};

#endif // FIELDCONFIG_H
