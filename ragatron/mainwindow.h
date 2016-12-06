#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/


#include <QMainWindow>
#include <QStringList>

#include "globals.h"
#include "html5game.h"

#include <QPushButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString hyperLinkStyle;
    QString validChoiceStyle;

    QPushButton *generateDNLink();


    QList<HTML5Game *> html5GameList;

    void statusBarMessage(const QString &msg, int timeout = 3000, bool override = false);
    void gameButtonChecks(HTML5Game *game);


public slots:
    void html5Unpack(xml_t xml);
    void html5Pack(xml_t xml);
private slots:

    void gotoDanNagleTwitter();
    void gotoRagatron();
    void on_launchUnpackFolderButton_clicked();
    void on_unpackPathButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
