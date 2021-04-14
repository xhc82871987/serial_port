#pragma once
#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#include <QtWidgets/QMainWindow>
#include <qdebug.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qstandarditemmodel.h>
#include <QItemSelectionModel>
#include <QMap>
#include <qtimer.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui/ui_MainWindow.h"
#include "DataUtil.h"
#include "AddDirectiveDlg.h"

//消息类型
enum MSG_TYPE {
    OK = 0,
    ERROR = 1
};

/**
 * 主窗口类
 */
class MainWindow : public QMainWindow {

    Q_OBJECT

private:

    Ui_MainWindow ui;

    bool opened = false;    //串口是否打开

    QMap<QString, QString> frameMap;    //数据帧名称-内容映射

    QTimer* pSendTimer = nullptr;       //定时发送数据

    DataUtil dataUtil;  //数据处理对象

    QSerialPort serialPort;	//串口类

    QJsonDocument configDoc;    //配置文件信息

    QStandardItemModel* pModel = nullptr;    //显示保存的帧数据的列表模型

    AddDirectiveDlg *pDlg = nullptr;        //添加指令的对话框

public:

	MainWindow(QWidget *parent = Q_NULLPTR);

	~MainWindow();

private:

    /**
     * 初始化程序
     */
    void init();

    // 初始化数据帧列表
    void initFrameTree();

    //加载配置文件
    void loadConfig(QString fileName = QString("./config.json"));

    //保存配置文件
    void saveConfig(QJsonDocument &config);

    //通过点击右方的条目，向发送框插入数据
    void insertFrame(const QModelIndex& index);

    //打开串口
    void openSerialPort();

    /**
     * 设置串口属性
     * @param baudRate 波特率
     * @param dataBits 数据位
     * @param parity 校验位
     * @param stopBits 停止位
     * @param flowControl 流控
     */
    void setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl);

    //关闭串口
    void closeSerialPort();

    //向data中加入校验码
    void fillCheckcode(QByteArray& data);

    /**
     * 显示消息
     * @param label 消息显示组件
     * @param msg 消息内容
     * @param type 消息类型
     */
    void showMsg(QLabel* label, QString& msg, int type);

	/*--------------------------------- SLOTS ---------------------------------*/
private slots:

    //发送数据
    void sendData();

    //读取数据
    void readData();

    //打开按钮事件
    void on_openBtn_clicked();

    //清空接收框按钮
    void on_clearReceiveBtn_clicked();

    //导入配置文件
    void on_addDirectiveAction_triggered();

    //导入配置文件
    void on_refreshPortAction_triggered();

    //导入配置文件
    void on_importConfigAction_triggered();

    //导入配置文件
    void on_exportConfigAction_triggered();

    //发送按钮事件
    void on_sendDataBtn_clicked(bool checked);

    //项目调试项被单击,在发送窗口中显示数据
    void on_frameTreeView_clicked(const QModelIndex& index);

    //项目调试项被双击,直接发送数据
    void on_frameTreeView_doubleClicked(const QModelIndex& index);

};

#endif

