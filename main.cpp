/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/

#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QDebug>


int main(int argc, char *argv[])
{

    int WIDTH = 500;
    int HEIGHT = 400;

    int screenWidth;
    int screenHeight;

    int x, y;

    QApplication a(argc, argv);
    MainWindow w;


    QDesktopWidget *desktop = QApplication::desktop();

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    x = (screenWidth - WIDTH) / 2;
    y = (screenHeight - HEIGHT) / 2;

    w.resize(WIDTH, HEIGHT);
    w.move( x, y );


    QFile file(":ragatron.css");
    if(file.open(QFile::ReadOnly)) {
       QString StyleSheet = QLatin1String(file.readAll());

       //qDebug() << "stylesheet: " << StyleSheet;
       a.setStyleSheet(StyleSheet);
    }


    w.show();

    return a.exec();
}
