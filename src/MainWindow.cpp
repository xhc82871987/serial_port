#include "MainWindow.h"
#include <qfiledialog.h>
#include <qfile.h>
#include <qdir.h>
#include <QStandardPaths>
#include <qdatetime.h>

MainWindow::MainWindow(QWidget* parent) {
	ui.setupUi(this);
	init();
}

MainWindow::~MainWindow() {
	//if (pSerialPort != nullptr) {
	//	if (pSerialPort->isOpen()) {
	//		pSerialPort->clear();
	//		pSerialPort->close();
	//		delete pSerialPort;
	//	}
	//}
}

void MainWindow::init() {
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	for (int i = 0; i < ports.size(); i++) {
		ui.portBox->addItem(ports[i].portName());
	}
	//加载配置文件
	loadConfig(QString::fromUtf8("./config.json"));
}

void MainWindow::initFrameTree(QJsonArray& projects) {
	QStandardItemModel* model = new QStandardItemModel(ui.frameTreeView);
	model->setHorizontalHeaderLabels(QStringList() << tr("选择数据"));     //设置列头
	for (int i = 0; i < projects.size(); i++) {
		QJsonObject project = projects[i].toObject();
		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QString projectName = project.value("name").toString();
		QStandardItem* item1 = new QStandardItem(projectName);
		items1.append(item1);
		model->appendRow(items1);
		//加载二级节点，即帧数据
		QJsonArray frames = project.value("frames").toArray();
		for (int j = 0; j < frames.size(); j++) {
			QJsonObject frame = frames[j].toObject();
			QString frameName = frame.value("name").toString();
			QString data = frame.value("data").toString();
			QList<QStandardItem*> items2;
			QStandardItem* item3 = new QStandardItem(frameName);
			items2.append(item3);
			item1->appendRow(items2);
			QString key = projectName + tr("_") + frameName;
			frameMap.insert(key, data);
		}
	}
	ui.frameTreeView->setModel(model);
}

void MainWindow::loadConfig(QString& fileName, bool saveFile) {
	QFile configFile(fileName);
	if (configFile.exists()) {
		if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QByteArray buffer = configFile.readAll();
			QString txt = QString::fromLocal8Bit(buffer.data());
			configFile.close();	//用完关掉
			QJsonParseError parseError;
			QJsonDocument document = QJsonDocument::fromJson(txt.toStdString().data(), &parseError);
			if (parseError.error == QJsonParseError::NoError) {
				//加载已经配置好的数据帧
				initFrameTree(document.array());
				if (saveFile) {
					QFile saveFile(QString::fromUtf8("./config.json"));
					if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
						saveFile.write(buffer);
						saveFile.close();
					}
				}
			} else {
				showMsg(ui.portStatusLabel, tr("文件解析失败"), ERROR);
			}
		} else {
			showMsg(ui.portStatusLabel, tr("文件打开失败"), ERROR);
		}
	}
}

void MainWindow::sendData() {
	if (pSerialPort != nullptr && pSerialPort->isOpen()) {
		QString data = ui.sendTextEdit->toPlainText();
		if (data.isEmpty()) {
			showMsg(ui.sendMsgLabel, tr("发送数据不能为空"), ERROR);
			return;
		}
		qDebug() << "send: " << ui.sendTextEdit->toPlainText();
		QByteArray buffer;
		//判断发送数据的方式
		if (ui.sASCIIRadioBtn->isChecked()) {
			buffer.append(data);
		} else if (ui.sHexRadioBtn->isChecked()) {
			bool ok = dataUtil.hex2Ascii(data, buffer);
			if (!ok) {
				showMsg(ui.sendMsgLabel, tr("非法的数据"), ERROR);
				return;
			}
		}
		//计算校验码
		fillCheckcode(buffer);
		//判断是否加入回车换行
		if (ui.addNewLineCheckBox->isChecked()) {
			buffer.append(0x0d);
			buffer.append(0x0a);
		}
		//发送数据
		pSerialPort->sendData(buffer);
	} else {
		showMsg(ui.portStatusLabel, tr("串口未打开"), ERROR);
	}
}

void MainWindow::insertFrame(const QModelIndex& index) {
	QString key = index.parent().data().toString();
	key.append("_").append(index.data().toString());
	ui.sendTextEdit->clear();
	ui.sendTextEdit->appendPlainText(frameMap.value(key));
}

void MainWindow::fillCheckcode(QByteArray& data) {
	//获取校验位起始和终止位
	int begin = ui.checkBeginSpinBox->value();
	int end = ui.checkEndSpinBox->value();
	//判断校验类型
	if (ui.crc16modbusCheckRadioBtn->isChecked()) {
		dataUtil.crc16ModbusCheck(data, begin, end);
	} else if (ui.bccCheckRadioBtn->isChecked()) {
		dataUtil.bccCheck(data, begin, end);
	} else if (ui.lrcRadioButton->isChecked()) {
		dataUtil.lrcCheck(data, begin, end);
	} else if (ui.bd2CheckRadioBtn->isChecked()) {
		dataUtil.bd2Check(data);
	}
}

void MainWindow::showMsg(QLabel* label, QString& msg, int type) {
	label->setText(msg);
	if (type == OK) {
		label->setStyleSheet(QString::fromUtf8("color: rgb(85, 170, 127);"));
	} else {
		label->setStyleSheet(QString::fromUtf8("color: red;"));
	}
}

void MainWindow::onResponse(QString data) {
	QString buffer;
	//判断显示方式
	if (ui.rASCIIRadioBtn->isChecked()) {
		//以ASCII方式显示
		buffer = data;
	} else if(ui.rHexRadioBtn->isChecked()) {
		//以Hex方式显示
		buffer = data.toUtf8().toHex(' ');
	}
	buffer.prepend("Receive: ");
	//判断是否显示时间
	if (ui.showTimeBox->isChecked()) {
		QDateTime now = QDateTime::currentDateTime();
		buffer.prepend(now.toString("[yyyy/MM/dd hh:mm:ss]"));
	}
	ui.receiveTextEdit->appendPlainText(buffer);
}

void MainWindow::onDataSendSuccess(QByteArray data) {
	showMsg(ui.sendMsgLabel, tr("发送成功"), OK);
	if (ui.sHexRadioBtn->isChecked()) {
		data = data.toHex(' ');
	}
	//判断是否要显示发送的数据
	if (ui.showSendDataBox->isChecked()) {
		QDateTime now = QDateTime::currentDateTime();
		QByteArray sData;
		sData.append(now.toString("[yyyy/MM/dd hh:mm:ss]"));
		sData.append("Send: ");
		sData.append(data);
		ui.receiveTextEdit->appendPlainText(sData);
	}
}

void MainWindow::on_openBtn_clicked() {
	//删除已经打开的串口
	if (pSerialPort != nullptr) {
		if (pSerialPort->isOpen()) {
			pSerialPort->clear();
			pSerialPort->close();
			delete pSerialPort;
		}
	}
	//初始化串口
	pSerialPort = new SerialPort(this);
	QString portName = ui.portBox->currentText();
	pSerialPort->setPortName(portName);
	if (pSerialPort->open(QIODevice::ReadWrite)) {
		int baudRate = ui.baudRateBox->currentText().toInt();
		int dataBits = ui.dataBitBox->currentIndex();
		int parity = ui.parityBox->currentIndex();
		int stopBits = ui.stopBitBox->currentIndex();
		int flowControl = ui.flowControlBox->currentIndex();
		pSerialPort->setPortProperty(baudRate, dataBits, parity, stopBits, flowControl);
		connect(pSerialPort, SIGNAL(readyRead()), pSerialPort, SLOT(readData()));
		connect(pSerialPort, SIGNAL(response(QString)), this, SLOT(onResponse(QString)));
		connect(pSerialPort, SIGNAL(sendSuccess(QByteArray)), this, SLOT(onDataSendSuccess(QByteArray)));
		showMsg(ui.portStatusLabel, tr("串口已打开"), OK);
		ui.openBtn->setDisabled(true);
	} else {
		showMsg(ui.portStatusLabel, tr("串口打开失败"), ERROR);
	}
}

void MainWindow::on_closeBtn_clicked() {
	if (pSerialPort != nullptr) {
		if (pSerialPort->isOpen()) {
			pSerialPort->clear();
			pSerialPort->close();
			delete pSerialPort;
		}
	}
	ui.portStatusLabel->setText(tr("串口已关闭"));
	ui.portStatusLabel->setStyleSheet(QString::fromUtf8("color: red;"));
	ui.openBtn->setDisabled(false);
}

void MainWindow::on_clearReceiveBtn_clicked() {
	ui.receiveTextEdit->clear();
}

void MainWindow::on_importConfigBtn_clicked() {
	//选择文件
	 //获取桌面路径
	QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	//获取保存的文件路径/文件名
	QString fileName = QFileDialog::getOpenFileName(this, tr("请选择配置文件"), dir, "JSON files (*.json)");
	loadConfig(fileName, true);
}

void MainWindow::on_sendDataBtn_clicked(bool checked) {
	qDebug() << "sendBtn: " << checked;
	if (ui.timerSendBox->isChecked()) {
		if (checked) {
			ui.sendDataBtn->setText(tr("停止"));
			sendData();
			//初始化Timer
			if (pSendTimer == nullptr) {
				pSendTimer = new QTimer();
				connect(pSendTimer, SIGNAL(timeout()), this, SLOT(sendData()));
			}
			pSendTimer->start(ui.timerSendSpinBox->value());
		} else {
			ui.sendDataBtn->setText(tr("发送"));
			//结束定时操作
			pSendTimer->stop();
		}
	} else {
		ui.sendDataBtn->setChecked(false);
		//发送数据
		sendData();
	}
}

void MainWindow::on_frameTreeView_clicked(const QModelIndex& index) {
	insertFrame(index);
}

void MainWindow::on_frameTreeView_doubleClicked(const QModelIndex& index) {
	insertFrame(index);
	sendData();
}
