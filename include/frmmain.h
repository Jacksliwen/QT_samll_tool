#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <qeventloop.h>
namespace Ui
{
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;
    QNetworkAccessManager *manager_;
    QEventLoop loop_;

private slots:
    void initForm();
    void initStyle();
    void initTranslator();
    void on_base_button_text2base_clicked();
    void on_base_button_choose_clicked();
    void on_base_button_pic2base_clicked();
    void on_base_button_base2pic_clicked();
    void on_base_button_clear_clicked();
    void on_base_button_base2text_clicked();
    void on_base_button_download_clicked();
    void on_button_consul_search_clicked();
    void on_button_deregiest_selected_clicked();
    void on_button_deregiest_all_off_clicked();

    void GetSuccessResp(const QString& resp);
    void on_treeWidget_nodes_itemClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_clicked();
    void on_treeWidget_nodes_itemEntered(QTreeWidgetItem *item, int column);
    void on_vlc_pushButton_clicked();
    void on_vlc_pushButton_2_clicked();
    void on_vlc_pushButton_3_clicked();
    void on_tabWidget_currentChanged(int index);
};

#endif // FRMMAIN_H
