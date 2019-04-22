#include "fieldconfig.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPoint>
#include <QMenu>
#include <QDebug>

#include "fieldconfigdelegate.h"
#include "config.h"


using config::FIELDS;
using config::FIELD;
using config::Field;
using config::MESSAGE;


QSize FieldConfig::size_(550, 300);


FieldConfig::FieldConfig(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    uiInit();
}

void FieldConfig::uiInit()
{
    QTreeView *pTreeView = new QTreeView();
    pTreeView->setEditTriggers(QTreeView::NoEditTriggers);
    pTreeView->setSelectionBehavior(QTreeView::SelectRows);
    pTreeView->setSelectionMode(QTreeView::SingleSelection);
    pTreeView->setAlternatingRowColors(true);
    pTreeView->setFocusPolicy(Qt::NoFocus);
    pTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    QHeaderView *pHeader = pTreeView->header();
    pHeader->setHighlightSections(true);                //列头点击时字体变粗，去掉该效果
    pHeader->setDefaultAlignment(Qt::AlignCenter);      //列头文字默认居中对齐
    pHeader->setDefaultSectionSize(100);                //默认列宽100
    pHeader->setStretchLastSection(true);               //最后一列自适应宽度
    pHeader->setSortIndicator(0,Qt::AscendingOrder);    //按第1列升序排序

    QStringList headers;
    headers << QStringLiteral("number")    // 01 - 128
            << QStringLiteral("name")
            << QStringLiteral("attribute") // nx ansx n or ans
            << QStringLiteral("format")    // none LLLVAR LLVAR
            << QStringLiteral("type");     // ASCII BCD BINARY or BYTE


    QStandardItemModel *pModel = new QStandardItemModel();

    auto message = config::config.getMessages().begin();
    for (auto it = message->second.begin(); it != message->second.end(); ++it)
    {
        setTreeItems(it->second, pModel->invisibleRootItem());
    }

    pModel->setHorizontalHeaderLabels(headers);
    pTreeView->setModel(pModel);
    pTreeView->setColumnWidth(0, 80);
    pTreeView->setColumnWidth(1, 120);

    FieldConfigDelegate *delegate = new FieldConfigDelegate();
    pTreeView->setItemDelegate(delegate);

    QPushButton *pButtonEdit = new QPushButton("edit");
    QPushButton *pButtonComplete = new QPushButton("complete");
    pButtonComplete->setVisible(false);
    QPushButton *pButtonCancel = new QPushButton("cancel");
    pButtonCancel->setVisible(false);
    QHBoxLayout *pHBox = new QHBoxLayout;
    pHBox->addStretch();
    pHBox->addWidget(pButtonEdit);
    pHBox->addWidget(pButtonComplete);
    pHBox->addWidget(pButtonCancel);


    QVBoxLayout *pVbox = new QVBoxLayout();
    pVbox->addLayout(pHBox);
    pVbox->addWidget(pTreeView);

    this->setLayout(pVbox);

    connect(pButtonEdit, &QPushButton::clicked, this, [pTreeView, pButtonEdit, pButtonComplete, pButtonCancel] () {
        pButtonEdit->setVisible(false);
        pButtonComplete->setVisible(true);
        pButtonCancel->setVisible(true);

        pTreeView->setEditTriggers(QTreeView::CurrentChanged);
    });
    connect(pButtonComplete, &QPushButton::clicked, this,
            [this, message, pModel, pTreeView, pButtonEdit, pButtonComplete, pButtonCancel] () {
        FIELDS fields = getConfigFields(pModel->invisibleRootItem());

        config::config.upDataMessage(MESSAGE(message->first, fields));

        pButtonEdit->setVisible(true);
        pButtonComplete->setVisible(false);
        pButtonCancel->setVisible(false);
        pTreeView->setEditTriggers(QTreeView::NoEditTriggers);
    });
    connect(pButtonCancel, &QPushButton::clicked, this,
            [this, headers, pTreeView, pModel, pButtonEdit, pButtonComplete, pButtonCancel] () {
        pModel->clear();

        auto message = config::config.getMessages().begin();
        for (auto it = message->second.begin(); it != message->second.end(); ++it)
        {
            setTreeItems(it->second, pModel->invisibleRootItem());
        }
        pModel->setHorizontalHeaderLabels(headers);
        pTreeView->setColumnWidth(0, 80);
        pTreeView->setColumnWidth(1, 120);

        pButtonEdit->setVisible(true);
        pButtonComplete->setVisible(false);
        pButtonCancel->setVisible(false);
        pTreeView->setEditTriggers(QTreeView::NoEditTriggers);
    });

    connect(pTreeView, &QTreeView::customContextMenuRequested, this, &FieldConfig::slotTreeMenu);

    pTreeView_ = pTreeView;
    pModel_ = pModel;
}

void FieldConfig::setTreeItems(const config::Field &field, QStandardItem *parentItem)
{
    QList<QStandardItem*> items;
    QStandardItem* itemNo = new QStandardItem(QStringLiteral("%1").arg(field.no));
    QStandardItem* itemName = new QStandardItem(field.name);
    QStandardItem* itemAttribute = new QStandardItem(field.attribute);
    QStandardItem* itemFormat = new QStandardItem(field.format);
    QStandardItem* itemType = new QStandardItem(field.type);
    items << itemNo << itemName << itemAttribute << itemFormat << itemType;

    parentItem->appendRow(items);    //二级节点挂在一级的第1列节点上

    for (auto it = field.sub.begin(); it != field.sub.end(); ++it)
    {
        setTreeItems(it->second, itemNo);
    }
}

void FieldConfig::slotTreeMenu(const QPoint &pos)
{
    if (pTreeView_->editTriggers() == QTreeView::NoEditTriggers)
    {
        return;
    }

    QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

    QMenu menu;
    menu.setStyleSheet(qss);    //可以给菜单设置样式

    QModelIndex index = pTreeView_->indexAt(pos);      //当前点击的元素的index
    index = index.sibling(index.row(), 0); //该行的第1列元素的index

    if (index.isValid())
    {
        menu.addAction(QStringLiteral("add subfield"), [this, &index] () {
            slotAddSubItem(index);
        });
        menu.addAction(QStringLiteral("insert field"), [this, &index] () {
            slotInsertItem(index);
        });
        menu.addAction(QStringLiteral("delete filed"), [this, &index] () {
            slotDeleteItem(index);
        });

        menu.exec(QCursor::pos());
    }
    else
    {
        menu.addAction(QStringLiteral("add field"), [this] () {
            QList<QStandardItem*> items;
            QStandardItem* itemNo = new QStandardItem("(none)");
            QStandardItem* itemName = new QStandardItem("(none)");
            QStandardItem* itemAttribute = new QStandardItem("(none)");
            QStandardItem* itemFormat = new QStandardItem("(none)");
            QStandardItem* itemType = new QStandardItem("(none)");
            items << itemNo << itemName << itemAttribute << itemFormat << itemType;
            pModel_->appendRow(items);
        });
        menu.exec(QCursor::pos());
    }
}

void FieldConfig::slotInsertItem(const QModelIndex &index)
{
    QList<QStandardItem*> row;
    QStandardItem* itemNo = new QStandardItem();
    QStandardItem* itemName = new QStandardItem();
    QStandardItem* itemAttribute = new QStandardItem();
    QStandardItem* itemFormat = new QStandardItem();
    QStandardItem* itemType = new QStandardItem();
    row << itemNo << itemName << itemAttribute << itemFormat << itemType;

    if (index.parent().isValid())
    {
        pModel_->itemFromIndex(index.parent())->insertRow(index.row(), row);
    }
    else
    {
        pModel_->invisibleRootItem()->insertRow(index.row(), row);
    }
}

void FieldConfig::slotAddSubItem(const QModelIndex &index)
{
    QList<QStandardItem*> row;
    QStandardItem* itemNo = new QStandardItem();
    QStandardItem* itemName = new QStandardItem();
    QStandardItem* itemAttribute = new QStandardItem();
    QStandardItem* itemFormat = new QStandardItem();
    QStandardItem* itemType = new QStandardItem();
    row << itemNo << itemName << itemAttribute << itemFormat << itemType;

    pModel_->itemFromIndex(index)->appendRow(row);
}

void FieldConfig::slotDeleteItem(const QModelIndex &index)
{
    pModel_->removeRow(index.row(), index.parent());
//    if (index.parent().isValid())
//    {
//        pModel_->removeRow(index.row(), index.parent());
//    }
//    else
//    {
//        pModel_->removeRow(index.row());
//    }
}

FIELDS FieldConfig::getConfigFields(QStandardItem *pRootItem)
{
    FIELDS fields;

    for (int i = 0;; i++)
    {
        QStandardItem *pNode1 = pRootItem->child(i);
        if (!pNode1)
            break;

        QStandardItem *pNode2 = pRootItem->child(i, 1);
        QStandardItem *pNode3 = pRootItem->child(i, 2);
        QStandardItem *pNode4 = pRootItem->child(i, 3);
        QStandardItem *pNode5 = pRootItem->child(i, 4);

        qDebug() << pNode1->text().toDouble() << ' '
                 << pNode2->text() << ' '
                 << pNode3->text() << ' '
                 << pNode4->text() << ' '
                 << pNode5->text();

        Field field = { pNode1->text().toDouble(), pNode2->text(), pNode3->text(), pNode4->text(), pNode5->text() };

        field.sub = getConfigFields(pNode1);

        fields.insert(FIELD(pNode1->text().toDouble(), field));
    }

    return fields;
}

void FieldConfig::show()
{
//    QPoint pt = static_cast<QWidget*>(parent())->cursor().pos();
//    qDebug() << pt.x() << ' ' << pt.y();

    QSize size = static_cast<QWidget*>(parent())->size();
    QPoint pt = static_cast<QWidget*>(parent())->pos();

    move(pt.x() + ((size.width() - size_.width()) >> 1),
         pt.y() + ((size.height() - size_.height()) >> 1));
    resize(size_);
    QWidget::show();
}

void FieldConfig::closeEvent(QCloseEvent *event)
{
    size_ = size();

    QWidget::closeEvent(event);
}



