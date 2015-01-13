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
    static QString byteArrayToHex(QByteArray data);

    QString hyperLinkStyle;
    QString validChoiceStyle;

    QPushButton *generateDNLink();

    HTML5Game WizardsLizard;
    HTML5Game LavaBlade;
    bool successfulUnpackWL;
    bool successfulUnpackLB;

    void statusBarMessage(const QString &msg, int timeout = 3000, bool override = false);
private slots:
    void on_wizardpathButton_clicked();

    void on_unpackPathButton_clicked();

    void on_unpackButton_clicked();

    void on_repackButton_clicked();
    void gotoDanNagleTwitter();
    void gotoRagatron();

    void on_launchUnpackFolderButton_clicked();

    void on_lavabladepathButton_clicked();

    void on_repackButtonLavaBlade_clicked();

    void on_unpackLavaBladeButton_clicked();

private:
    Ui::MainWindow *ui;
    bool copySupportFiles(QString path, HTML5Game *h5game);
};

#endif // MAINWINDOW_H
