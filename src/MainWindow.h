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

    QMap<QString, QString> frameMap;    //����֡����-����ӳ��

    QTimer* pSendTimer = nullptr;       //��ʱ��������

    DataUtil dataUtil;  //���ݴ������

    QSerialPort serialPort;	//������

    QJsonDocument configDoc;    //�����ļ���Ϣ

    QStandardItemModel* pModel = nullptr;    //��ʾ�����֡���ݵ��б�ģ��

    AddDirectiveDlg *pDlg = nullptr;        //���ָ��ĶԻ���

public:

	MainWindow(QWidget *parent = Q_NULLPTR);

	~MainWindow();

private:

    /**
     * ��ʼ������
     */
    void init();

    // ��ʼ������֡�б�
    void initFrameTree();

    //���������ļ�
    void loadConfig(QString fileName = QString("./config.json"));

    //���������ļ�
    void saveConfig(QJsonDocument &config);

    //ͨ������ҷ�����Ŀ�����Ϳ��������
    void insertFrame(const QModelIndex& index);

    //�򿪴���
    void openSerialPort();

    /**
     * ���ô�������
     * @param baudRate ������
     * @param dataBits ����λ
     * @param parity У��λ
     * @param stopBits ֹͣλ
     * @param flowControl ����
     */
    void setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl);

    //�رմ���
    void closeSerialPort();

    //��data�м���У����
    void fillCheckcode(QByteArray& data);

    /**
     * ��ʾ��Ϣ
     * @param label ��Ϣ��ʾ���
     * @param msg ��Ϣ����
     * @param type ��Ϣ����
     */
    void showMsg(QLabel* label, QString& msg, int type);

	/*--------------------------------- SLOTS ---------------------------------*/
private slots:

    //��������
    void sendData();

    //��ȡ����
    void readData();

    //�򿪰�ť�¼�
    void on_openBtn_clicked();

    //��ս��տ�ť
    void on_clearReceiveBtn_clicked();

    //���������ļ�
    void on_addDirectiveAction_triggered();

    //���������ļ�
    void on_refreshPortAction_triggered();

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

#endif

