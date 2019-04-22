#include "widget.h"

#include <QToolBar>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "fieldconfig.h"
#include "func/function.h"

#define sysprintf mk::func::sysprintA


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    uiInit();
}

Widget::~Widget()
{

}

void Widget::uiInit()
{
    // ui
    QTextEdit *pInputTextEdit = new QTextEdit();
    QTextEdit *pOutputTextEdit = new QTextEdit();
    QPushButton *pDecode = new QPushButton("-->");
    connect(pDecode, SIGNAL(clicked(bool)), this, SLOT(decode()));

    QToolBar *pToolBar = new QToolBar();
    pToolBar->addAction(QString("fields"), this, SLOT(configField()));
    pToolBar->addAction(QString("select"), this, SLOT(selectField()));

    QHBoxLayout *pHBox = new QHBoxLayout();
    pHBox->addWidget(pInputTextEdit);
    pHBox->addWidget(pDecode);
    pHBox->addWidget(pOutputTextEdit);

    QVBoxLayout *pVBox = new QVBoxLayout();
    pVBox->addWidget(pToolBar);
    pVBox->addLayout(pHBox);

    this->setLayout(pVBox);

    this->resize(800, 400);

    // other
    pInputTextEdit_ = pInputTextEdit;
    pOutputTextEdit_ = pOutputTextEdit;
}

void Widget::decode()
{
    qDebug() << "decode";

    QString sInputText = pInputTextEdit_->toPlainText().remove(QRegExp("\\s"));
//    qDebug() << sInputText;

    sysprintf(sInputText.toStdString().c_str());

    std::vector<uchar> vData = mk::func::Hex2Bcd(sInputText.toStdString().c_str());

    sysprintf("size = %d", vData.size());

    mk::func::HexPrintA(&vData[0], vData.size());
}

void Widget::configField()
{
    qDebug() << "configField";
    FieldConfig *f = new FieldConfig(this);
    f->show();
}

void Widget::selectField()
{
    qDebug() << "selectField";
}
