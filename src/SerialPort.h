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

	QSerialPort serial;	//串口类

public:

	SerialPort(QObject *parent = Q_NULLPTR);

	/**
	 * 发送数据
	 * @param portName	串口名称
	 * @param data	待发送的数据
	 * @param timeout	发送数据最大等待时间
	 */
	void sendData(QByteArray &data, int timeout = 30000);

	/**
	 * 设置串口属性
	 */
	void setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl);

signals:

	//提交接收到的数据到GUI线程
	void response(QString data);

	//数据发送成功
	void sendSuccess(QByteArray data);

private slots:

	//读取数据
	void readData();

};





#endif //!_SERIAL_PORT_THREAD_H_