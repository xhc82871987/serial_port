#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include <QtWidgets/QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    //初始化主窗口
    QApplication app(argc, argv);
    //初始化主窗口
    MainWindow window;
    window.show();
    //结束欢迎屏
    return app.exec();
}
