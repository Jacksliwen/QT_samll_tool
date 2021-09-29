#pragma execution_character_set("utf-8")
#include "frmmain.h"
#include "ui_frmmain.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qtranslator.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include "base64.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <memory>
#include <functional>
#include <QToolTip>
#include "qelapsedtimer.h"
#include "common/log.h"

frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("./logo.ico"));

    this->initForm();
}

frmMain::~frmMain()
{
    if(nullptr != manager_)
        manager_->deleteLater();

    delete ui;
}

void frmMain::initForm()
{
    this->initStyle();
    this->initTranslator();
    ui->tabWidget->setCurrentIndex(0);
}

void frmMain::initStyle()
{
    //加载样式表
    QString qss;
    //QFile file(":/qss/psblack.css");
    //QFile file(":/qss/flatwhite.css");
    QFile file(":/qss/lightblue.css");
    if (file.open(QFile::ReadOnly)) {
#if 1
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        //in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        qss = list.join("\n");
#else
        //用readAll读取默认支持的是ANSI格式,如果不小心用creator打开编辑过了很可能打不开
        qss = QLatin1String(file.readAll());
#endif
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        qApp->setStyleSheet(qss);
        file.close();
    }
    manager_ = new QNetworkAccessManager();
    ui->treeWidget_nodes->setMouseTracking(true);
    ui->treeWidget_nodes->setHeaderLabel("节点信息");
    QT_LOG::outedit = ui->textEdit_conusl_log;
    //初始化日志
    QT_LOG::logInit("log/smalltool.log");

}

void frmMain::initTranslator()
{
    //加载鼠标右键菜单翻译文件
    QTranslator *translator1 = new QTranslator(qApp);
    translator1->load(":/image/qt_zh_CN.qm");
    qApp->installTranslator(translator1);

    //加载富文本框鼠标右键菜单翻译文件
    QTranslator *translator2 = new QTranslator(qApp);
    translator2->load(":/image/widgets.qm");
    qApp->installTranslator(translator2);
}

void frmMain::on_base_button_text2base_clicked()
{
    QString text = ui->base_text->toPlainText().trimmed();
    if (!text.isEmpty()) {
        ui->base_ret->setText(Base64::textToBase64(text));
    }

}

void frmMain::on_base_button_base2text_clicked()
{
    QString text = ui->base_ret->toPlainText().trimmed();
    if (!text.isEmpty()) {
        ui->base_text->setText(Base64::base64ToText(text));
    }

}

void frmMain::on_base_button_choose_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "", "图片(*.png *.jpeg *.bmp *.jpg)");
    if (!fileName.isEmpty()) {
        ui->base_text_file->setText(fileName);
        QPixmap pix(fileName);
        pix = pix.scaled(ui->base_image->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->base_image->setPixmap(pix);
    }

}

void frmMain::on_base_button_pic2base_clicked()
{
    QString fileName = ui->base_text_file->text().trimmed();
    if (!fileName.isEmpty()) {
        ui->base_ret->setText(Base64::imageToBase64(QImage(fileName)));
    }

}

void frmMain::on_base_button_base2pic_clicked()
{
    QString text = ui->base_ret->toPlainText().trimmed();
    if (!text.isEmpty()) {
        QPixmap pix = QPixmap::fromImage(Base64::base64ToImage(text));
        pix = pix.scaled(ui->base_image->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->base_image->setPixmap(pix);
    }

}

void frmMain::on_base_button_clear_clicked()
{
    ui->base_text->clear();
    ui->base_text_file->clear();
    ui->base_ret->clear();
    ui->base_image->clear();

}

void frmMain::on_base_button_download_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", "", "图片(*.png *.jpeg *.bmp *.jpg)");
    if (!fileName.isEmpty()) {
        QPixmap p = ui->base_image->grab(ui->base_image->rect());
        QImage img = p.toImage();
        img.save(fileName);
    }

}

void frmMain::GetSuccessResp(const QString& resp)
{
    ui->treeWidget_nodes->clear();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(resp.toLatin1());
    QJsonArray jsonArray = jsonDocument.array();
    //    QString value = jsonObject.value("status").toString();

    for (auto iter : jsonArray) {
        QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget_nodes);
        group->setText(0, iter.toObject()["Addr"].toString());
        group->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        group->setCheckState(0, Qt::PartiallyChecked);

        QNetworkRequest request;
        request.setUrl(QUrl("http://" + iter.toObject()["Addr"].toString() + ":8500/v1/agent/checks"));

        QNetworkReply* reply = manager_->get(request);//发送get请求
        QEventLoop loop_;
        connect(reply, &QNetworkReply::finished,&loop_, &QEventLoop::quit);
        loop_.exec();

        QJsonDocument jsonDocument1 = QJsonDocument::fromJson(reply->readAll());
        for(auto each_iter : jsonDocument1.object()){
            QTreeWidgetItem *subItem = new QTreeWidgetItem(group);
            subItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            subItem->setText(0, each_iter.toObject()["ServiceID"].toString());
            subItem->setToolTip(0, "ServiceName: "+ each_iter.toObject()["ServiceName"].toString() + "\nStatus: " + each_iter.toObject()["Status"].toString());

            if("critical" == each_iter.toObject()["Status"].toString()){
                subItem->setTextColor(0, QColor(255,0,0));
                subItem->setCheckState(0, Qt::Checked);
            }else {
                subItem->setTextColor(0, QColor(0,150,0));
                subItem->setCheckState(0, Qt::Unchecked);
            }
        }
        reply->deleteLater();
    }
    ui->treeWidget_nodes->expandAll();

}

void frmMain::on_button_consul_search_clicked()
{
    do{
        auto addr  = ui->lineEdit_consul_addr->text();
        if(addr.isEmpty()){
            ui->textEdit_conusl_log->append("请填写Consul Addr");
            break;
        }

        QNetworkRequest request;
        request.setUrl(QUrl("http://" + addr+ "/v1/agent/members"));
        QNetworkReply* reply = manager_->get(request);
        connect(reply, &QNetworkReply::finished,&loop_, &QEventLoop::quit);
        loop_.exec();

        if(reply->error() != QNetworkReply::NoError){
            ui->textEdit_conusl_log->append("请填写Consul Addr");
        }
        GetSuccessResp(reply->readAll());
    }while(0);
}

void frmMain::on_button_deregiest_selected_clicked()
{
    QTreeWidgetItemIterator it(ui->treeWidget_nodes);
    std::string addr  = ui->lineEdit_consul_addr->text().toStdString();
    auto pos = addr.find(":");
    std::string port;
    if(pos != std::string::npos){
        port =  addr.substr(pos + 1);
    }
    while (*it) {
        for( int i=0; i < (*it)->childCount(); ++i ){
            if( Qt::Checked == (*it)->child(i)->checkState(0)){
                QNetworkRequest request;
                QUrl url("http://" + (*it)->text(0) + ":"+ QString::fromStdString(port) +"/v1/agent/service/deregister/" +(*it)->child(i)->text(0) );
                request.setUrl(url);
                QByteArray putData;
                QNetworkReply* reply = manager_->put(request,putData);//发送put请求
                connect(reply, &QNetworkReply::finished,&loop_, &QEventLoop::quit);
                loop_.exec();
                reply->deleteLater();
            }
        }
        ++it;
    }
    on_button_consul_search_clicked();
}

void frmMain::on_button_deregiest_all_off_clicked()
{
    on_button_consul_search_clicked();
    on_button_deregiest_selected_clicked();
}

void frmMain::on_treeWidget_nodes_itemClicked(QTreeWidgetItem *item, int column)
{
    if( column!=0 )
        return;

    // 设置子对象的勾选状态
    for( int i=0;i<item->childCount();++i )
        item->child(i)->setCheckState(0,item->checkState(0));

    // 更新父对象的勾选状态
    QTreeWidgetItem* p = item->parent();
    if( p!=0 ){
        p->setCheckState(0,item->checkState(0));
        for( int i=0;i<p->childCount();++i )
            if( p->child(i)->checkState(0)!=item->checkState(0) ){
                p->setCheckState(0,Qt::PartiallyChecked);
                return;
            }
    }
}


void frmMain::on_pushButton_clicked()
{
    ui->treeWidget_nodes->clear();
    ui->textEdit_conusl_log->clear();

}

void frmMain::on_treeWidget_nodes_itemEntered(QTreeWidgetItem *item, int column)
{
    QToolTip::showText(QCursor::pos(),item->toolTip(column));
    return ;
}

void frmMain::on_vlc_pushButton_clicked()
{
    if (ui->vlc_pushButton->text() == "播放") {
        ui->vlc_pushButton->setText("停止");
        QString url = ui->vlc_comboBox->currentText().trimmed();
        ui->vlcwidget->setUrl(url);
        ui->vlcwidget->open();
    } else {
        ui->vlc_pushButton->setText("播放");
        ui->vlcwidget->close();
    }
}

void frmMain::on_vlc_pushButton_2_clicked()
{
    if (ui->vlc_pushButton_2->text() == "播放") {
        ui->vlc_pushButton_2->setText("停止");
        QString url = ui->vlc_comboBox_2->currentText().trimmed();
        ui->vlcwidget_2->setUrl(url);
        ui->vlcwidget_2->open();
    } else {
        ui->vlc_pushButton_2->setText("播放");
        ui->vlcwidget_2->close();
    }

}

void frmMain::on_vlc_pushButton_3_clicked()
{
    if (ui->vlc_pushButton_3->text() == "播放") {
        ui->vlc_pushButton_3->setText("停止");
        QString url = ui->vlc_comboBox_3->currentText().trimmed();
        ui->vlcwidget_3->setUrl(url);
        ui->vlcwidget_3->open();
    } else {
        ui->vlc_pushButton_3->setText("播放");
        ui->vlcwidget_3->close();
    }

}

void frmMain::on_tabWidget_currentChanged(int index)
{
    switch(index){
    case 0:    QT_LOG::outedit = ui->textEdit_conusl_log;
        break;
    case 1:    QT_LOG::outedit = ui->video_textEdit;
        break;
    }
}
