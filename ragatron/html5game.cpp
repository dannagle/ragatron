/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/

#include <QCryptographicHash>
#include <QDesktopServices>
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QDomDocument>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>


#include "html5game.h"

HTML5Game::HTML5Game(QObject *parent) :
    QObject(parent)
{
    gameJSArray.clear();
    packPath.clear();
    unpackPath.clear();
}

QString HTML5Game::titleClean()
{
    QString gameTitle = xml.title;
    gameTitle.replace(" ", "_");
    gameTitle.replace("'", "");
    gameTitle.replace("&", "");
    return gameTitle;
}


bool deletefile(QString filename) {
    QFile file(filename);
    return file.remove();
}

QByteArray readFile(QString filename) {

    QByteArray returnArray;
    returnArray.clear();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        returnArray = file.readAll();
        file.close();
    }

    return returnArray;
}

//used by recursive directory copy.
static bool rmDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
        return true;
    foreach(const QFileInfo &info, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            if (!rmDir(info.filePath()))
                return false;
        } else {
            if (!dir.remove(info.fileName()))
                return false;
        }
    }
    QDir parentDir(QFileInfo(dirPath).path());
    return parentDir.rmdir(QFileInfo(dirPath).fileName());
}


//recursive directory copy.
static bool cpDir(const QString &srcPath, const QString &dstPath)
{

    //QDEBUG() << srcPath << dstPath;

    //rmDir(dstPath);
    QDir parentDstDir(QFileInfo(dstPath).path());
    if (!QFile::exists(dstPath) && !parentDstDir.mkdir(QFileInfo(dstPath).fileName())) {
        QDEBUG() <<"false" << dstPath;
        return false;
    }

    QDir srcDir(srcPath);
    foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        QString srcItemPath = srcPath + "/" + info.fileName();
        QString dstItemPath = dstPath + "/" + info.fileName();
        if (info.isDir()) {
            if (!cpDir(srcItemPath, dstItemPath)) {
                QDEBUG() <<"false" << srcItemPath << dstItemPath;
                return false;
            }
        } else if (info.isFile()) {
            if(QFile(dstItemPath).exists()) {
                //QDEBUG() << "Skipping copy of " << dstItemPath;
                return true;
            }
            if (!QFile::copy(srcItemPath, dstItemPath)) {
                QDEBUG() <<"false";
                return false;
            } else {
                //QDEBUG() << "Copying of " << dstItemPath;
            }
        } else {
            QDEBUG() << "Unhandled item" << info.filePath() << "in cpDir";
        }
    }
    return true;
}

QString grabXMLData(QDomElement &xmlDoc, QString tag) {

    QDomNode n;
    QDomElement e;

    QDomNodeList nodes = xmlDoc.elementsByTagName(tag);

    if(nodes.count() > 0) {
        n = nodes.at(0);
        e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            return e.text();
        }

    }

    return "";
}

#define GRABXML(var) xml.var = grabXMLData(e, # var)
#define GRABHACKXML(var) gameHack.var = grabXMLData(e, # var)


bool HTML5Game::scanXML(QString xmlFile) {

    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    QDomDocument xmlDoc;
    int i=0;
    hack_t gameHack;

    QFile file(xmlFile);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!xmlDoc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();


    QString platform;
#ifdef __APPLE__
    platform = "mac";
#else
    platform = "windows";
#endif

    QDomElement docElem = xmlDoc.documentElement();
    xml.title = grabXMLData(docElem, "title");


    QDomNodeList dList = docElem.elementsByTagName(platform);
    if(dList.count() > 0) {
        QDomElement e = dList.at(0).toElement();
        GRABXML(defaultpath);
        xml.defaultpath.replace("~", QDir::homePath());
        GRABXML(md5);
        GRABXML(target);
        GRABXML(outputname);
        GRABXML(launchfile);
    }

    QDEBUGVAR(xml.launchfile);


    if(xml.launchfile.isEmpty()) {
        xml.launchfile = "index.html";
    }

    dList = docElem.elementsByTagName("hack");

    for(i=0; i< dList.count(); i++) {
        QDomElement e = dList.at(i).toElement();
        GRABHACKXML(category);
        GRABHACKXML(platform);
        GRABHACKXML(id);
        GRABHACKXML(name);
        GRABHACKXML(wintarget);
        GRABHACKXML(mactarget);
#ifdef __APPLE__
        gameHack.target = gameHack.mactarget;
#else
        gameHack.target = gameHack.wintarget;
#endif


        GRABHACKXML(search);
        GRABHACKXML(replace);

        if(gameHack.platform == platform || gameHack.platform == "all") {
            xml.hacks.append(gameHack);
        } else {
            QDEBUG() << "Discarding hack" << gameHack.name;
        }

    }

    xml.md5 = xml.md5.toLower();

    if(!QFile::exists(xml.defaultpath)) {
        QString test  = settings.value(xml.md5 +"_defaultpath").toString();
        if(!test.isEmpty()) {
            xml.defaultpath = test;
        }
    }

    backupPath = QString(SETTINGSPATH + xml.md5);

    if(QFile::exists(xml.defaultpath)) {
        cpDir(xml.defaultpath, backupPath);
    }



    cheatTab  = new CheatUI();
    cheatTab->xml = xml;
    cheatTab->generate();

    return true;
}


bool HTML5Game::isJSValid()
{

    unsigned int i = 0;


    if(gameJSArray.isEmpty()) {

        gameJSArray = readFile(backupPath + "/" + xml.target);


    }

    if(gameJSArray.isEmpty()) {
        QDEBUG() << "Game not installed. ";

        return false;
    }

    QByteArray fileHash;
    fileHash = QCryptographicHash::hash(gameJSArray, QCryptographicHash::Md5);
    QString MD5 = fileHash.toHex().toLower();
    if(MD5 == xml.md5) {

        QDEBUGVAR(unpackPath);
        return true;
    }

    gameJSArray.clear();

    QDEBUG() << "MD5 was" << MD5 <<"it should be" << xml.md5;
    return false;
}

bool HTML5Game::canRepack()
{    
    return QFile::exists(unpackPath + "/" + xml.target);
}

bool HTML5Game::unpack() {




    rmDir(unpackPath);
    QFile::copy(":Ragatron_instructions.txt", unpackPath + "/Ragatron_instructions.txt");
    cpDir(backupPath, unpackPath);

    return canRepack();
}

bool HTML5Game::repack()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    if(!canRepack()) {
        QDEBUG() << "I am not unpacked. I cannot do this";
    }

    hack_t gameHack;

    QDEBUGVAR(packPath);
    rmDir(packPath);
    QDir mpath;
    mpath.mkdir(packPath);
    QHash<QString, QByteArray> otherFiles;
    otherFiles.clear();

    if(!cpDir(backupPath, packPath)) {
        QDEBUG() <<"Could not recursive copy" << backupPath <<"to" << packPath;
        return false;
    }


    foreach(gameHack, xml.hacks) {
        if(settings.value(gameHack.id).toBool()) {
            //QDEBUG() << "Hack" <<gameHack.name  << gameHack.search << gameHack.replace;

            QString otherFilesPath = packPath + "/" + gameHack.target;


            if(!QFile::exists(otherFilesPath)) {
                QDEBUG() << otherFilesPath <<"does not exist!";
                continue;
            }

            otherFiles[otherFilesPath] = readFile(otherFilesPath);

            QDEBUG() << otherFilesPath << (otherFiles[otherFilesPath].size());

            QString otherFileString(otherFiles[otherFilesPath]);
            otherFileString.replace(gameHack.search, gameHack.replace);
            otherFiles[otherFilesPath] = otherFileString.toLatin1();
            QFile otherFile(otherFilesPath);
            if(otherFile.open(QIODevice::WriteOnly)) {
                otherFile.write(otherFileString.toLatin1());
                otherFile.close();
            } else {
                QDEBUG() << "failed to open File!" << unpackPath + "/" + gameHack.target;
                return false;
            }
        }
    }

    QDEBUGVAR(backupPath);
    QDEBUGVAR(packPath);


    return true;
}

