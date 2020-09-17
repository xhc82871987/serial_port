#pragma once
#ifndef DATA_UTIL_H_
#define DATA_UTIL_H_
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif

#include<qobject.h>

class DataUtil : public QObject {

	Q_OBJECT

public:

	DataUtil();

	/**
	 * ��HEX��ʽ������ת����ASCII���ݷ���
	 * @param data ��ת��������
	 * @param result ת�����
	 * @return ����ת�����
	 */
	bool hex2Ascii(QString &data, QByteArray &result);

	/**
	 * ��data�е����ݽ���CRC16/MODBUSУ��
	 * @param data ���������
	 * @param begin ת�����
	 * @param end У�����λ��������
	 */
	void crc16ModbusCheck(QByteArray &data, int begin = 0, int end = 0);

	/**
	 * ��data�е����ݽ���BCCУ��
	 * @param data ���������
	 * @param begin ת�����
	 * @param end У�����λ��������
	 */
	void bccCheck(QByteArray& data, int begin = 0, int end = 0);

	/**
	 * ��data�е����ݽ���LRCУ��
	 * @param data ���������
	 * @param begin ת�����
	 * @param end У�����λ��������
	 */
	void lrcCheck(QByteArray& data, int begin = 0, int end = 0);

	/**
	 * ��װ��������
	 * @param data ���������
	 * @param begin ת�����
	 * @param end У�����λ��������
	 */
	void bd2Check(QByteArray& data, int begin = 1, int end = 0);

};


#endif //!DATA_UTIL_H_