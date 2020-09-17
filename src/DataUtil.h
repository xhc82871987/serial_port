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
	 * 将HEX形式的数据转换成ASCII数据返回
	 * @param data 待转换的数据
	 * @param result 转换结果
	 * @return 返回转换结果
	 */
	bool hex2Ascii(QString &data, QByteArray &result);

	/**
	 * 对data中的数据进行CRC16/MODBUS校验
	 * @param data 输入的数据
	 * @param begin 转换结果
	 * @param end 校验结束位（倒数）
	 */
	void crc16ModbusCheck(QByteArray &data, int begin = 0, int end = 0);

	/**
	 * 对data中的数据进行BCC校验
	 * @param data 输入的数据
	 * @param begin 转换结果
	 * @param end 校验结束位（倒数）
	 */
	void bccCheck(QByteArray& data, int begin = 0, int end = 0);

	/**
	 * 对data中的数据进行LRC校验
	 * @param data 输入的数据
	 * @param begin 转换结果
	 * @param end 校验结束位（倒数）
	 */
	void lrcCheck(QByteArray& data, int begin = 0, int end = 0);

	/**
	 * 封装北斗数据
	 * @param data 输入的数据
	 * @param begin 转换结果
	 * @param end 校验结束位（倒数）
	 */
	void bd2Check(QByteArray& data, int begin = 1, int end = 0);

};


#endif //!DATA_UTIL_H_