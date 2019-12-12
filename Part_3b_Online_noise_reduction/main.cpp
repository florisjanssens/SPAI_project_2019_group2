#include <QCoreApplication>
#include <QMediaPlayer>
#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFile>
#include <iostream>
#include <fstream>

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
