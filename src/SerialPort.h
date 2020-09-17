#pragma once
#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>


class SerialPort : public QSerialPort {
	
	Q_OBJECT

private:

	QSerialPort serial;	//������

public:

	SerialPort(QObject *parent = Q_NULLPTR);

	/**
	 * ��������
	 * @param portName	��������
	 * @param data	�����͵�����
	 * @param timeout	�����������ȴ�ʱ��
	 */
	void sendData(QByteArray &data, int timeout = 30000);

	/**
	 * ���ô�������
	 */
	void setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl);

signals:

	//�ύ���յ������ݵ�GUI�߳�
	void response(QString data);

	//���ݷ��ͳɹ�
	void sendSuccess(QByteArray data);

private slots:

	//��ȡ����
	void readData();

};





#endif //!_SERIAL_PORT_THREAD_H_