#ifndef HTML5GAME_H
#define HTML5GAME_H
/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/


#include <QObject>
#include <QStringList>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include "globals.h"

class HTML5Game : public QObject
{
    Q_OBJECT
public:
    explicit HTML5Game(QObject *parent = 0);


    QString pathString;
    QByteArray gameByteArray;
    QByteArray packageJSONByteArray;
    QByteArray indexHTMLByteArray;
    QByteArray nwEXEByteArray;
    QByteArray appNWByteArray;
    QString gameMD5;
    QString appNW;
    QString nwEXE;
    QStringList gameFileList;
    QStringList zipFileList;
    quint64 exeBYTELocation;
    QString repackName;
    QString exeNameOriginal;
    QString packPath;
    QString indexHTMLBackup;
    QString packageJSONBackup;


    bool isEXEValid();
    bool hasBackup();
    bool extractEXE();
    bool saveBackup(QString path);
    bool canUnpack();
signals:

public slots:

};

#endif // HTML5GAME_H
