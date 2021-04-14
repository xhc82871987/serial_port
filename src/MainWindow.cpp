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
	closeSerialPort();
	if (pModel != nullptr) {
		pModel->deleteLater();
	}
	if (pSendTimer != nullptr) {
		pSendTimer->deleteLater();
	}
}

void MainWindow::init() {
	closeSerialPort();
	connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
	//刷新串口列表
	on_refreshPortAction_triggered();
	//加载配置文件
	loadConfig();
	initFrameTree();
}

void MainWindow::initFrameTree() {
	QJsonArray& frameArr = this->configDoc.array();
	if (pModel == nullptr) {
		pModel = new QStandardItemModel(ui.frameTreeView);
		ui.frameTreeView->setModel(pModel);
	} else {
		pModel->clear();
	}
	//设置列头
	pModel->setHorizontalHeaderLabels(QStringList() << tr("选择数据"));
	for (int i = 0; i < frameArr.size(); i++) {
		QJsonObject project = frameArr[i].toObject();
		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QString projectName = project.value("name").toString();
		QStandardItem* item1 = new QStandardItem(projectName);
		items1.append(item1);
		pModel->appendRow(items1);
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
}

void MainWindow::loadConfig(QString fileName) {
	QFile configFile(fileName);
	if (configFile.exists()) {
		if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QByteArray buffer = configFile.readAll();
			QString txt = QString::fromLocal8Bit(buffer.data());
			configFile.close();	//用完关掉
			//解析JSON数据
			QJsonParseError parseError;
			this->configDoc = QJsonDocument::fromJson(txt.toStdString().data(), &parseError);
			if (parseError.error != QJsonParseError::NoError) {
				showMsg(ui.portStatusLabel, tr("文件解析失败"), ERROR);
			}
		} else {
			showMsg(ui.portStatusLabel, tr("文件打开失败"), ERROR);
		}
	}
}

void MainWindow::saveConfig(QJsonDocument& config) {
	QFile saveFile(QString::fromUtf8("./config.json"));
	if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QString txt = QString(config.toJson());
		saveFile.write(txt.toLocal8Bit());
		saveFile.close();
	}
}

void MainWindow::insertFrame(const QModelIndex& index) {
	QString key = index.parent().data().toString();
	key.append("_").append(index.data().toString());
	ui.sendTextEdit->clear();
	ui.sendTextEdit->appendPlainText(frameMap.value(key));
}

void MainWindow::openSerialPort() {
	//关闭已经打开的串口
	closeSerialPort();
	QString portName = ui.portBox->currentText();
	serialPort.setPortName(portName);
	if (serialPort.open(QIODevice::ReadWrite)) {
		int baudRate = ui.baudRateBox->currentText().toInt();
		int dataBits = ui.dataBitBox->currentIndex();
		int parity = ui.parityBox->currentIndex();
		int stopBits = ui.stopBitBox->currentIndex();
		int flowControl = ui.flowControlBox->currentIndex();
		setPortProperty(baudRate, dataBits, parity, stopBits, flowControl);
		//connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
		showMsg(ui.portStatusLabel, tr("串口已打开"), OK);
	} else {
		showMsg(ui.portStatusLabel, tr("串口打开失败"), ERROR);
	}
}

void MainWindow::setPortProperty(int baudRate, int dataBits, int parity, int stopBits, int flowControl) {
	serialPort.setBaudRate(baudRate);
	switch (dataBits) {
	case 0: serialPort.setDataBits(QSerialPort::Data5); break;
	case 1: serialPort.setDataBits(QSerialPort::Data6); break;
	case 2: serialPort.setDataBits(QSerialPort::Data7); break;
	case 3: serialPort.setDataBits(QSerialPort::Data8); break;
	default:serialPort.setDataBits(QSerialPort::UnknownDataBits); break;
	}
	switch (parity) {
	case 0: serialPort.setParity(QSerialPort::NoParity); break;
	case 1: serialPort.setParity(QSerialPort::EvenParity); break;
	case 2: serialPort.setParity(QSerialPort::OddParity); break;
	case 3: serialPort.setParity(QSerialPort::SpaceParity); break;
	case 4: serialPort.setParity(QSerialPort::MarkParity); break;
	default: serialPort.setParity(QSerialPort::UnknownParity); break;
	}
	switch (stopBits) {
	case 0: serialPort.setStopBits(QSerialPort::OneStop); break;
	case 1: serialPort.setStopBits(QSerialPort::OneAndHalfStop); break;
	case 2: serialPort.setStopBits(QSerialPort::TwoStop); break;
	default: serialPort.setStopBits(QSerialPort::UnknownStopBits); break;
	}
	switch (flowControl) {
	case 0: serialPort.setFlowControl(QSerialPort::NoFlowControl); break;
	case 1: serialPort.setFlowControl(QSerialPort::HardwareControl); break;
	case 2: serialPort.setFlowControl(QSerialPort::SoftwareControl); break;
	default: serialPort.setFlowControl(QSerialPort::UnknownFlowControl); break;
	}
}

void MainWindow::closeSerialPort() {
	//删除已经打开的串口
	if (serialPort.isOpen()) {
		serialPort.clear();
		serialPort.close();
	}
	showMsg(ui.portStatusLabel, tr("请选择串口"), OK);
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

void MainWindow::sendData() {
	if (serialPort.isOpen()) {
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
		//sendData(buffer);
		serialPort.write(buffer);
		serialPort.flush();
		if (serialPort.waitForBytesWritten(30000)) {
			showMsg(ui.sendMsgLabel, tr("发送成功"), OK);
			//判断是否要显示发送的数据
			if (ui.showSendDataBox->isChecked()) {
				QDateTime now = QDateTime::currentDateTime();
				QByteArray sData;
				sData.append(now.toString("[yyyy/MM/dd hh:mm:ss]"));
				sData.append("Send: ");
				if (ui.sHexRadioBtn->isChecked()) {
					sData.append(buffer.toHex(' '));
				}
				ui.receiveTextEdit->appendPlainText(sData);
			}
		}
	} else {
		showMsg(ui.portStatusLabel, tr("串口未打开"), ERROR);
	}
}

void MainWindow::readData() {
	QByteArray buf = serialPort.readAll();
	while (serialPort.waitForReadyRead(100)) {
		buf.append(serialPort.readAll());
	}
	QString buffer;
	//判断显示方式
	if (ui.rASCIIRadioBtn->isChecked()) {
		//以ASCII方式显示
		buffer = buf;
	} else if (ui.rHexRadioBtn->isChecked()) {
		//以Hex方式显示
		buffer = QString(buf).toUtf8().toHex(' ');
	}
	buffer.prepend("Receive: ");
	//判断是否显示时间
	if (ui.showTimeBox->isChecked()) {
		QDateTime now = QDateTime::currentDateTime();
		buffer.prepend(now.toString("[yyyy/MM/dd hh:mm:ss]"));
	}
	ui.receiveTextEdit->appendPlainText(buffer);
}

void MainWindow::on_openBtn_clicked() {
	if (opened) {
		//串口已经打开，关闭串口
		closeSerialPort();
		opened = false;
		ui.openBtn->setStyleSheet(QString::fromUtf8("QPushButton {background-color: rgb(85, 170, 255);}"));
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.openBtn->setIcon(icon);
		ui.openBtn->setText(tr("打开"));

	} else {
		//串口未打开，打开串口
		openSerialPort();
		if (serialPort.isOpen()) {
			opened = true;
			ui.openBtn->setStyleSheet(QString::fromUtf8("QPushButton {background-color: rgb(255, 0, 0);}"));
			QIcon icon;
			icon.addFile(QString::fromUtf8(":/images/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.openBtn->setIcon(icon);
			ui.openBtn->setText(tr("关闭"));
		}
	}
}

void MainWindow::on_clearReceiveBtn_clicked() {
	ui.receiveTextEdit->clear();
}

void MainWindow::on_addDirectiveAction_triggered() {
	if (this->pDlg == nullptr) {
		pDlg = new AddDirectiveDlg(this);
	}
	//加载配置文件
	loadConfig();
	int val = pDlg->showDlg(this->configDoc);
	if (val == 1) {
		//保存配置信息
		saveConfig(this->configDoc);
		initFrameTree();
	}
}

void MainWindow::on_refreshPortAction_triggered() {
	ui.portBox->clear();
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	for (int i = 0; i < ports.size(); i++) {
		ui.portBox->addItem(ports[i].portName());
	}
}

void MainWindow::on_importConfigAction_triggered() {
	//选择文件
	 //获取桌面路径
	QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	//获取保存的文件路径/文件名
	QString fileName = QFileDialog::getOpenFileName(this, tr("请选择配置文件"), dir, "JSON files (*.json)");
	//加载并保存配置文件
	loadConfig(fileName);
	saveConfig(this->configDoc);
	initFrameTree();
}

void MainWindow::on_exportConfigAction_triggered() {
	//选择文件
	 //获取桌面路径
	QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	//获取保存的文件路径/文件名
	QString saveName = QFileDialog::getSaveFileName(this, tr("请选择配置文件保存位置"), dir, "JSON files (*.json)");
	if (!saveName.isEmpty()) {
		//打开配置文件
		QFile configFile(QString::fromUtf8("./config.json"));
		if (configFile.exists() && configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			configFile.copy(saveName);
			//QByteArray buffer = configFile.readAll();
			configFile.close(); //关闭配置文件
			//QFile saveFile(saveName);
		}
	}
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
