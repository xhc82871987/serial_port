#include "DataUtil.h"

DataUtil::DataUtil() {}

bool DataUtil::hex2Ascii(QString& data, QByteArray& result) {
	//若数据为空，则直接返回false
	if (data.isEmpty()) {
		return false;
	}
	//以空格为分隔符将数据进行分割
	QStringList strList = data.split(" "); 
	for (int i = 0; i < strList.size(); i++) {
		bool ok = true;
		int val = strList[i].toInt(&ok, 16);
		result.push_back((uchar)val);
		if (!ok) {
			//若解析错误，情况result，结束循环
			result.clear();
			return false;
		}
	}
	return true;
}

void DataUtil::crc16ModbusCheck(QByteArray& data, int begin, int end) {
	ushort crcValue = 0xFFFF;
	ushort tmp;
	for (int i = begin; i < data.size() - end; ++i) {
		crcValue ^= (uchar)data[i];
		for (int j = 0; j < 8; ++j) {
			tmp = crcValue & 0x0001;
			crcValue >>= 1;
			if (tmp) {
				crcValue ^= 0xA001;
			}
		}
	}
	data.push_back(uchar(crcValue >> 8));
	data.push_back(uchar(crcValue));
}

void DataUtil::bccCheck(QByteArray& data, int begin, int end) {
	uchar bcc = 0;
	for (int i = begin; i < data.size() - end; i++) {
		bcc ^= data[i];
	}
	data.push_back(bcc);
}

void DataUtil::lrcCheck(QByteArray& data, int begin, int end) {
	uchar lrc = 0;
	for (int i = begin; i < data.size() - end; i++) {
		lrc += data[i];
	}
	lrc = (uchar)((lrc ^ 0xFF) + 1);
	data.push_back(lrc);
}

void DataUtil::bd2Check(QByteArray& data, int begin, int end) {
	uchar bcc = 0;
	for (int i = begin; i < data.size() - end; i++) {
		bcc ^= data[i];
	}
	QByteArray result = QString::number((int)bcc, 16).toUtf8();
	if (result.size() == 1) {
		result.push_front(0x30);
	}
	data.push_back("*");
	data.append(result.toUpper());
	data.push_back(0x0d);
	data.push_back(0x0a);
}
