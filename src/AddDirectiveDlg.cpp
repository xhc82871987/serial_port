#include "AddDirectiveDlg.h"
#include <qmessagebox.h>

AddDirectiveDlg::AddDirectiveDlg(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f){
	ui.setupUi(this);
}

int AddDirectiveDlg::showDlg(QJsonDocument& config) {
	//��ʼ������
	ui.parentComboBox->clear();
	ui.parentComboBox->addItem(tr("None"));
	QJsonArray &projects = config.array();
	for (int i = 0; i < projects.size(); i++) {
		QJsonObject &project = projects[i].toObject();
		ui.parentComboBox->addItem(project.value("name").toString());
	}
	int val = this->exec();
	//��������
	if (val == 1) {
		//�ж����ݰ������Ƿ�Ϊ��
		QString frameName = ui.nameLineEdit->text();
		if (!frameName.isEmpty()) {
			//����Ŀ����û��ѡ��Ļ�������Ϊ�������Ŀ
			if (ui.parentComboBox->currentIndex() == 0) {
				QJsonArray dataArr;
				QJsonObject obj;
				obj.insert("name", frameName);
				obj.insert("frames", dataArr);
				projects.push_back(obj);
			} else {
				//���ݰ���Ŀ����
				QString projectName = ui.parentComboBox->currentText();
				for (int i = 0; i < projects.size(); i++) {
					//��ȡԭ��Ŀ����
					QJsonObject& project = projects[i].toObject();
					//����ǰѡ�����Ŀ���ƺ͵�ǰ��Ŀ����һ������������ݵ���ǰ��Ŀ
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
							QMessageBox::critical(this, tr("����"), tr("���ݰ�����Ϊ��"), tr("�ر�"));
						}
						break;
					}
				}
			}
		} else {
			QMessageBox::critical(this, tr("����"), tr("���ݰ����Ʋ���Ϊ��"), tr("�ر�"));
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
