#pragma once
#ifndef _ADD_DIRECTIVE_DLG_H_
#define _ADD_DIRECTIVE_DLG_H_
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#include "ui/ui_AddDirectiveDlg.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>

class AddDirectiveDlg : public QDialog {

	Q_OBJECT

private:
	Ui_Dialog ui;

public:

	AddDirectiveDlg(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	//显示对话框
	int showDlg(QJsonDocument& config);

public slots:

	//确定按钮
	void on_confirmBtn_clicked();

	//取消按钮
	void on_cancelBtn_clicked();

};


#endif