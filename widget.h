#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>//请求
#include <QNetworkReply>//回复


#include <QCryptographicHash>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();



private slots:
    void on_btn_clicked();
    void read_reply(QNetworkReply * reply);

private:
    Ui::Widget *ui;
    QNetworkAccessManager* client;

};
#endif // WIDGET_H
