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
#include "ui_MainWindow.h"
#include "SerialPort.h"
#include "DataUtil.h"

//��Ϣ����
enum MSG_TYPE {
    OK = 0,
    ERROR = 1
};

/**
 * ��������
 */
class MainWindow : public QMainWindow {

    Q_OBJECT

private:

    Ui_MainWindow ui;

    bool opened = false;    //�����Ƿ��

    SerialPort* pSerialPort = nullptr;  //���ڹ���

    QMap<QString, QString> frameMap;    //����֡����-����ӳ��

    QTimer* pSendTimer = nullptr;       //��ʱ��������

    DataUtil dataUtil;  //���ݴ������

public:

	MainWindow(QWidget *parent = Q_NULLPTR);

	~MainWindow();

private:

    /**
     * ��ʼ������
     */
    void init();

    //��ʼ������֡�б�
    void initFrameTree(QJsonArray &projects);

    //���������ļ�
    void loadConfig(QString &fileName, bool saveFile = false);

    //ͨ������ҷ�����Ŀ�����Ϳ��������
    void insertFrame(const QModelIndex& index);

    //��data�м���У����
    void fillCheckcode(QByteArray &data);

    //�򿪴���
    void openSerialPort();

    //�رմ���
    void closeSerialPort();

    /**
     * ��ʾ��Ϣ
     */
    void showMsg(QLabel *label, QString &msg, int type);

	/*--------------------------------- SLOTS ---------------------------------*/
private slots:

    //��������
    void sendData();

    //��������
    void onResponse(QString data);

    //���ݷ��ͳɹ�
    void onDataSendSuccess(QByteArray data);

    //�򿪰�ť�¼�
    void on_openBtn_clicked();

    //��ս��տ�ť
    void on_clearReceiveBtn_clicked();

    //���������ļ�
    void on_importConfigAction_triggered();

    //���������ļ�
    void on_exportConfigAction_triggered();

    //���Ͱ�ť�¼�
    void on_sendDataBtn_clicked(bool checked);

    //��Ŀ���������,�ڷ��ʹ�������ʾ����
    void on_frameTreeView_clicked(const QModelIndex& index);

    //��Ŀ�����˫��,ֱ�ӷ�������
    void on_frameTreeView_doubleClicked(const QModelIndex& index);

};

#endif // !_MAIN_WINDOW_H_

