#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>


class QTextEdit;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void decode();
    void configField();
    void selectField();

private:
    void uiInit();

private:
    QTextEdit *pInputTextEdit_;
    QTextEdit *pOutputTextEdit_;
};

#endif // WIDGET_H
