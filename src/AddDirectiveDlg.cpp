#include "AddDirectiveDlg.h"
#include <qmessagebox.h>

AddDirectiveDlg::AddDirectiveDlg(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f){
	ui.setupUi(this);
}

int AddDirectiveDlg::showDlg(QJsonDocument& config) {
	//初始化界面
	ui.parentComboBox->clear();
	ui.parentComboBox->addItem(tr("None"));
	QJsonArray &projects = config.array();
	for (int i = 0; i < projects.size(); i++) {
		QJsonObject &project = projects[i].toObject();
		ui.parentComboBox->addItem(project.value("name").toString());
	}
	int val = this->exec();
	//保存数据
	if (val == 1) {
		//判断数据包名称是否为空
		QString frameName = ui.nameLineEdit->text();
		if (!frameName.isEmpty()) {
			//若项目名称没有选择的话，则认为是添加项目
			if (ui.parentComboBox->currentIndex() == 0) {
				QJsonArray dataArr;
				QJsonObject obj;
				obj.insert("name", frameName);
				obj.insert("frames", dataArr);
				projects.push_back(obj);
			} else {
				//数据包项目名称
				QString projectName = ui.parentComboBox->currentText();
				for (int i = 0; i < projects.size(); i++) {
					//获取原项目数据
					QJsonObject& project = projects[i].toObject();
					//若当前选择的项目名称和当前项目名称一致则添加新数据到当前项目
					if (project.value("name") == projectName) {
						QJsonArray& frames = project.value("frames").toArray();
						if (!ui.dataTextEdit->toPlainText().isEmpty()) {
							QJsonObject frame;
							frame.insert("name", ui.nameLineEdit->text());
							frame.insert("data", ui.dataTextEdit->toPlainText());
							frames.push_back(frame);
							project.insert("frames", frames);
							projects[i] = project;
						} else {
							QMessageBox::critical(this, tr("错误"), tr("数据包不能为空"), tr("关闭"));
						}
						break;
					}
				}
			}
		} else {
			QMessageBox::critical(this, tr("错误"), tr("数据包名称不能为空"), tr("关闭"));
		}
	}
	config.setArray(projects);
	return val;
}

void AddDirectiveDlg::on_confirmBtn_clicked() {
	this->accept();
}

void AddDirectiveDlg::on_cancelBtn_clicked() {
	this->reject();
}
