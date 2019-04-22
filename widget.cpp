#include "widget.h"

#include <QToolBar>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "fieldconfig.h"
#include "func/function.h"
#include "func/Exception.h"


#define sysprintf mk::func::sysprintA

using mk::func::Hex2Bcd;
using mk::func::Bcd2Hex;
using std::vector;


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

    sysprintf(sInputText.toStdString().c_str());

    pOutputTextEdit_->clear();
    decode(Hex2Bcd(
               sInputText.toStdString().c_str()));
}

void Widget::decode(std::vector<uchar> &data)
{
    if (data.size() < 10)
    {
        throw mk::ErrorException("length error");
    }

    pOutputTextEdit_->append(QString("消息类型: ") + Bcd2Hex(&data[0], 2).c_str());

    data.erase(data.begin(), data.begin() + 2);

    vector<uchar> bitmap = getBitmap(data);

}

vector<uchar> Widget::getBitmap(std::vector<uchar> &data)
{
    int loop = data[0] & 0x80 ? 16 : 8;

    for (int i = 0; i < loop; ++i)
    {
        for (int bit = 7; i >= 0; --bit)
        {

        }
    }

    return vector<uchar>();
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
