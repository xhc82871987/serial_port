#include "SerialPort.h"

SerialPort::SerialPort(QObject* parent) {
	if (this->isOpen()) {
		this->clear();
		this->close();
	}
}

void SerialPort::sendData(QByteArray &data, int timeout) {
	this->write(data);
	this->flush();
	if (this->waitForBytesWritten(timeout)) {
		emit sendSuccess(data);
	}
}

void SerialPort::setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl) {
	this->setBaudRate(baudRate);
	switch (dataBits) {
		case 0: this->setDataBits(QSerialPort::Data5); break;
		case 1: this->setDataBits(QSerialPort::Data6); break;
		case 2: this->setDataBits(QSerialPort::Data7); break;
		case 3: this->setDataBits(QSerialPort::Data8); break;
		default:this->setDataBits(QSerialPort::UnknownDataBits); break;
	}
	switch (parity) {
		case 0: this->setParity(QSerialPort::NoParity); break;
		case 1: this->setParity(QSerialPort::EvenParity); break;
		case 2: this->setParity(QSerialPort::OddParity); break;
		case 3: this->setParity(QSerialPort::SpaceParity); break;
		case 4: this->setParity(QSerialPort::MarkParity); break;
		default: this->setParity(QSerialPort::UnknownParity); break;
	}
	switch (stopBits) {
		case 0: this->setStopBits(QSerialPort::OneStop); break;
		case 1: this->setStopBits(QSerialPort::OneAndHalfStop); break;
		case 2: this->setStopBits(QSerialPort::TwoStop); break;
		default: this->setStopBits(QSerialPort::UnknownStopBits); break;
	}
	switch (flowControl) {
		case 0: this->setFlowControl(QSerialPort::NoFlowControl); break;
		case 1: this->setFlowControl(QSerialPort::HardwareControl); break;
		case 2: this->setFlowControl(QSerialPort::SoftwareControl); break;
		default: this->setFlowControl(QSerialPort::UnknownFlowControl); break;
	}
}

void SerialPort::readData() {
	QByteArray buf = this->readAll();
	while (this->waitForReadyRead(10)) {
		buf += this->readAll();
	}
	emit response(buf);
}
