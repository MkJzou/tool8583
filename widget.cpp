#include "widget.h"

#include <algorithm>

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
using mk::func::Bcd2Int;
using std::vector;
using std::copy;


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

    pOutputTextEdit_->append("<p><font color=\"#eeff00\">123</font></p>");
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

    QString log("位图: ");
    for (auto &nm : bitmap)
    {
        log += QString("%1 ").arg(nm);
    }
    pOutputTextEdit_->append(log);

    auto fields = config::config.getMessages().begin()->second;

    for (auto &nm : bitmap)
    {
        if (fields.find(nm) != fields.end())
        {
//                double no;
//                QString name;
//                QString attribute;
//                QString format;
//                QString type;
//                std::map<double, Field> sub;
            auto field = fields[nm];
            log = QString("%1 ").arg(field.no) + field.name + ': ';

            qDebug() << field.no << ' '
                     << field.name << ' '
                     << field.attribute << ' '
                     << field.format << ' '
                     << field.type << ' ';

            int varLen;
            if (field.format == "LLVAR")
            {
                sysprintf("%d", data.size());
                sysprintf("%02", data[0]);

                varLen = Bcd2Int(&data[0], 1);
                data.erase(data.begin(), data.begin() + 1);
            }
            else if (field.format == "LLLVAR")
            {
                sysprintf("%d", data.size());
                sysprintf("%02X %02X", data[0], data[1]);

                varLen = Bcd2Int(&data[0], 2);
                data.erase(data.begin(), data.begin() + 2);
            }
            else
            {
                int index = field.attribute.indexOf(QRegExp("(\\d+$)"));
                varLen = field.attribute.right(
                            field.attribute.length() - index).toInt();
            }

            qDebug() << "varLen = " << varLen;

            vector<uchar> varData;

            if (field.type == "ASCII")
            {
                std::copy(data.begin(), data.begin() + varLen, std::back_inserter(varData));
                varData.push_back(0);
                log += (char*)&varData[0];
                data.erase(data.begin(), data.begin() + varLen);
            }
            else
            {
                varLen = (varLen + 1) * 2 / 2;
                std::copy(data.begin(), data.begin() + varLen / 2, std::back_inserter(varData));
                log += Bcd2Hex(&varData[0], varData.size()).c_str();
                data.erase(data.begin(), data.begin() + varLen / 2);
            }

            pOutputTextEdit_->append(log);
        }
    }
}

vector<uchar> Widget::getBitmap(std::vector<uchar> &data)
{
    vector<uchar> bitmap;

    int loop = data[0] & 0x80 ? 16 : 8;

    for (int i = 0; i < loop; ++i)
    {
        for (int bit = 7; bit >= 0; --bit)
        {
            if ((data[i] >> bit) & 0x01)
            {
                bitmap.push_back(i * 8 + (8 - bit));
            }
        }
    }

    data.erase(data.begin(), data.begin() + loop);
    return bitmap;
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
