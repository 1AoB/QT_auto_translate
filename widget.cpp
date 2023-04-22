#include "widget.h"
#include "ui_widget.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrlQuery>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    client = new QNetworkAccessManager;
    //等待百度服务器给我们响应,去读取响应到的数据
    connect(client,
            &QNetworkAccessManager::finished,
            this,
            &Widget::read_reply);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_btn_clicked()
{
    //1.准备参数
    /*
    生成方法：
    Step1. 将请求参数中的 APPID(appid)， //平台查看
    翻译 query(q，注意为UTF-8编码)，//要翻译的文本
    随机数(salt)，//使用QDateTime获取当前时间作为随机数
    以及平台分配的密钥(可在管理控制台查看)
    按照 appid+q+salt+密钥的顺序拼接得到字符串 1。//QString src
    Step2. 对字符串 1 做 MD5 ，得到 32 位小写的 sign。//QString sign
    */
    QString from = ui->src_comboBox->currentText();
    QString q = ui->src_textEdit->toPlainText();

    QString to = ui->dst_comboBox->currentText();
    QString dstStr = ui->dst_textEdit->toPlainText();

    QString appid="20230422001651323";//APP ID：20230422001651323
    QString secret="pdMMwgq5nAPmpv6MTEU9";//密钥：pdMMwgq5nAPmpv6MTEU9
    QString salt = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");//随机字符串

    QString src = appid+q+salt+secret;
    QString sign = QCryptographicHash::hash(src.toUtf8(),
                                            QCryptographicHash::Md5).toHex();//对src进行MD5加密

    qDebug()<<from<<" "<<q<<" "<<to<<" "<<dstStr;
    qDebug()<<"src:"<<src;
    qDebug()<<"sign:"<<sign;

    QUrlQuery parame;
    parame.addQueryItem("q",q);
    parame.addQueryItem("from",from);
    parame.addQueryItem("to",to);
    parame.addQueryItem("appid",appid);
    parame.addQueryItem("salt",salt);
    parame.addQueryItem("sign",sign);

    //2.输入参数,进行翻译--接入百度AI翻译API---连接百度网络
    //1)发起请求
    QNetworkRequest request(QUrl("https://fanyi-api.baidu.com/api/trans/vip/translate"));

    //2)给请求设置头部信息
    //如使用 POST 方式，Content-Type 请指定为：application/x-www-form-urlencoded
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //3.输出参数,将翻译得到的结果显示到目标文本框中
    //客户端发起请求
    //请求需要的参数:参数使用QUrl  encode编码---->转成utf-8的编码格式
    client->post(request,parame.toString(QUrl::FullyEncoded).toUtf8());

    //等待Web服务器给我们响应,如何知道服务器给我们反馈?
    //看是否有对应的信号产生
}

void Widget::read_reply(QNetworkReply *reply)
{
    //读取应答
    //QString content = reply->readAll();
    //qDebug()<<content;

    //如何解析json? [key]得到value数据
    /*
    {
        "from": "en",
        "to": "zh",
        "trans_result": [
            {
                "src": "apple",
                "dst": "苹果"
            }
        ]
    }
    */
    //QJsonDocument::fromJson(reply->readAll())将读到的内容转成json文本内容,返回值是QJsonValue类型
    //object()就是把json文本变成了json对象,是QJsonValue类型的一个方法,可以获得一个QJsonObject对象
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
    //获取trans_result中的value
    if(json.contains("trans_result"))
    {
       //json.value("trans_result")与json["trans_result"]等价
       QJsonArray arry = json["trans_result"].toArray();//读取key是trans_result的QJsonArray数组
       qDebug()<< arry;
       //qDebug()<<"arry.size():"<<arry.size();
       for(int i = 0 ; i < arry.size() ; i++){
           //[{}]=====>[]括起来是数组,{}括起来是对象
           qDebug()<<"arry["<<i<<"]   :    "<<arry[i];//通过打印内容,可以知道QJsonArray数组中的每个元素的类型是QJsonValue类型
           qDebug()<<"arry["<<i<<"].toObject()   :    "<<arry[i].toObject();
           qDebug()<<"arry["<<i<<"].toObject().value('dst')    :    "<<arry[i].toObject().value("dst");
           //qDebug()<<arry[i].toObject().value("dst").toString();
           ui->dst_textEdit->setText(arry[i].toObject().value("dst").toString());
       }
    }else if(json.contains("error_code")){
        QMessageBox::information(this,"提示","翻译失败!");
        qDebug()<<"出错了";
    }
}





































