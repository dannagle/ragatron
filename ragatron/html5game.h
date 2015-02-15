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
#include "cheatui.h"
#include "hackstruct.h"

class HTML5Game : public QObject
{
    Q_OBJECT
public:
    explicit HTML5Game(QObject *parent = 0);

    xml_t xml;

    QByteArray gameByteArray;
    QByteArray packageJSONByteArray;
    QByteArray indexHTMLByteArray;
    QByteArray nwEXEByteArray;
    QByteArray appNWByteArray;
    quint64 exeBYTELocation;

    QString packPath;
    QString unpackPath;
    QString indexHTMLBackup;
    QString packageJSONBackup;
    QString backupPath;

    QString titleClean();

    bool isEXEValid();
    bool canRepack();
    bool scanXML(QString xmlFile);
    bool unpack();
    bool repack();


    CheatUI * cheatTab;

signals:

public slots:

};

#endif // HTML5GAME_H
