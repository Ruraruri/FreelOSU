#include "main.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    FreelOSU w;
    w.show();
    std::thread t(player::setup, &w);
    t.detach();
    return a.exec();
}
