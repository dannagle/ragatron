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

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"

#include "html5game.h"

HTML5Game::HTML5Game(QObject *parent) :
    QObject(parent)
{

    gameByteArray.clear();
    packageJSONByteArray.clear();
    indexHTMLByteArray.clear();
    nwEXEByteArray.clear();
    appNWByteArray.clear();
    exeBYTELocation = 0;

    packPath.clear();

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
        xml.app_nw_is_compressed = grabXMLData(e, "app_nw_is_compressed").toUInt();
        xml.app_nw_is_hidden = grabXMLData(e, "app_nw_is_hidden").toUInt();
        GRABXML(app_nw_bytelocation);
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
        GRABHACKXML(target);
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

    exeBYTELocation = xml.app_nw_bytelocation.toULongLong();



    indexHTMLBackup = SETTINGSPATH + xml.md5 + "/index.html.backup";
    packageJSONBackup = SETTINGSPATH + xml.md5 + "/package.json.backup";
    backupPath = QString(SETTINGSPATH + xml.md5);

    if(QFile::exists(xml.defaultpath)) {
        cpDir(xml.defaultpath, backupPath);
    }



    cheatTab  = new CheatUI();
    cheatTab->xml = xml;
    cheatTab->generate();

    return true;
}


bool HTML5Game::isEXEValid()
{

    unsigned int i = 0;
    if(gameByteArray.isEmpty() || nwEXEByteArray.isEmpty() || appNWByteArray.isEmpty()) {


        QDEBUGVAR(backupPath + "/" + xml.target);
        gameByteArray = readFile(backupPath + "/" + xml.target);


        QDEBUGVAR(gameByteArray.size());
        if(xml.app_nw_is_compressed && xml.app_nw_is_hidden) {
            for(i = 0; i < exeBYTELocation-1; i++) {
                nwEXEByteArray.append(gameByteArray[i]);
            }

            QDEBUGVAR(nwEXEByteArray.size());
            for(i = exeBYTELocation; i < gameByteArray.size(); i++) {
                appNWByteArray.append(gameByteArray[i]);
            }
            QDEBUGVAR(appNWByteArray.size());
        }

        if(xml.app_nw_is_compressed && !xml.app_nw_is_hidden) {

            appNWByteArray = gameByteArray;
            nwEXEByteArray.append('c'); //add char since checks for empty.
        }


    }

    if(gameByteArray.isEmpty()) {
        QDEBUG() << "Game not installed. ";

        return false;
    }

    QByteArray fileHash;
    fileHash = QCryptographicHash::hash(gameByteArray, QCryptographicHash::Md5);
    QString MD5 = fileHash.toHex().toLower();
    if(MD5 == xml.md5) {

        QDEBUGVAR(packageJSONBackup);
        QDEBUGVAR(indexHTMLBackup);
        QDEBUGVAR(unpackPath);

        packageJSONByteArray = readFile(packageJSONBackup);
        indexHTMLByteArray = readFile(indexHTMLBackup);

        return true;
    } else {
        gameByteArray.clear();
        nwEXEByteArray.clear();
        appNWByteArray.clear();
    }

    QDEBUG() << "MD5 was" << MD5 <<"it should be" << xml.md5;
    return false;
}

bool HTML5Game::canRepack()
{


    if(packageJSONByteArray.isEmpty()) {
        packageJSONByteArray = readFile(packageJSONBackup);
    }

    if(indexHTMLByteArray.isEmpty()) {

        indexHTMLByteArray = readFile(indexHTMLBackup);
    }

    if(!QFile(unpackPath).exists()) {
        QDEBUGVAR(unpackPath);
        return false;
    }

    QDEBUGVAR(packageJSONBackup);
    QDEBUGVAR(indexHTMLBackup);

    QDEBUGVAR(gameByteArray.size());


    if(!xml.app_nw_is_hidden) {
        nwEXEByteArray.append(1);
    }

    if(!xml.app_nw_is_compressed) {
        appNWByteArray.append(1);
    }

    QDEBUGVAR(nwEXEByteArray.size());
    QDEBUGVAR(appNWByteArray.size());
    QDEBUGVAR(packageJSONByteArray.size());
    QDEBUGVAR(indexHTMLByteArray.size());


    if(gameByteArray.isEmpty() || nwEXEByteArray.isEmpty() || appNWByteArray.isEmpty()
            ||

            (packageJSONByteArray.isEmpty() && !QFile::exists(unpackPath + "/package.nw/package.json")) ||


            (indexHTMLByteArray.isEmpty() && !QFile::exists(unpackPath + "/package.nw/index.html"))

            ) {

        return false;
    } else {


                return true;
    }

}

bool HTML5Game::unpack() {

    QString zipFile = unpackPath + "/app.nw";
    if(isEXEValid()) {
        QDEBUG() << "Yes, I can unpack";
        QDEBUGVAR(unpackPath);
        QDir mk;
        rmDir(unpackPath);
        mk.mkpath(unpackPath);
        QFile appnwFile (zipFile);
        if (xml.app_nw_is_compressed && appnwFile.open(QIODevice::WriteOnly)) {
             appnwFile.write(appNWByteArray);
             appnwFile.close();

             QStringList zipFileList = JlCompress::getFileList(zipFile);
             QuaZip zip(zipFile);
             if(zip.open(QuaZip::mdUnzip)) {
                 zip.close();
                 //zip is valid!
                 zipFileList = JlCompress::getFileList(zipFile);
                 JlCompress::extractFiles(zipFile, zipFileList, unpackPath);
                 deletefile(zipFile);
                 packageJSONByteArray = readFile(unpackPath + "/package.json");
                 indexHTMLByteArray = readFile(unpackPath + "/" + xml.launchfile);

                 QFile::copy(unpackPath + "/package.json",  packageJSONBackup);
                 QFile::copy(unpackPath + "/" + xml.launchfile, indexHTMLBackup);

                 deletefile(unpackPath + "/package.json");
                 deletefile(unpackPath + "/" + xml.launchfile);

                 QDEBUGVAR(packageJSONByteArray.size());
                 QDEBUGVAR(indexHTMLByteArray.size());

                 QFile::copy(":Ragatron_instructions.txt", unpackPath + "/Ragatron_instructions.txt");

                 return true;
             } else {
                 return false;
             }

        } else {
            QDEBUG() << "App is not compresssed. Just copy everything over...";

#ifdef __APPLE__
            cpDir(backupPath + "/Contents/Resources/app.nw/", unpackPath);
#else
            cpDir(backupPath + "/", unpackPath);
#endif
            QFile::copy(":Ragatron_instructions.txt", unpackPath + "/Ragatron_instructions.txt");
            QFile::copy(unpackPath + "/package.json",  packageJSONBackup);
            QFile::copy(unpackPath + "/" + xml.launchfile, indexHTMLBackup);
            packageJSONByteArray = readFile(packageJSONBackup);
            indexHTMLByteArray = readFile(indexHTMLBackup);


            //append dummy variables so isEmpty() checks are ok...
            nwEXEByteArray.append("a");
            appNWByteArray.append("a");

            return true;

        }
    } else {
        QDEBUG() << "No, cannot unpack";
    }


    return false;
}

bool HTML5Game::repack()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);


    hack_t gameHack;

    QDEBUGVAR(packPath);
    rmDir(packPath);
    QDir mpath;
    mpath.mkdir(packPath);
    QHash<QString, QByteArray> otherFiles;
    otherFiles.clear();

    if((packageJSONByteArray.size() > 0 && indexHTMLByteArray.size() > 0)

            || (QFile::exists(unpackPath + "/package.nw/package.json") && QFile::exists(unpackPath + "/package.nw/index.html"))

            ) {
        QDEBUG() << "We are unpacked. I can do this";
        QString packageJSONString(packageJSONByteArray);
        QString indexHTMLString(indexHTMLByteArray);

        foreach(gameHack, xml.hacks) {
            if(settings.value(gameHack.id).toBool()) {
                //QDEBUG() << "Hack" <<gameHack.name  << gameHack.search << gameHack.replace;
                if(gameHack.target == "package.json") {
                    packageJSONString.replace(gameHack.search, gameHack.replace);
                } else if(gameHack.target == xml.launchfile) {
                    indexHTMLString.replace(gameHack.search, gameHack.replace);
                } else {


                    QString otherFilesPath = unpackPath + "/" + gameHack.target;
                    if(!QFile::exists(otherFilesPath)) {
                        otherFilesPath = unpackPath + "/package.nw/" + gameHack.target;

                    }



#if __APPLE__
                    //read from the backup once
                if(otherFiles[otherFilesPath].isEmpty()) {
                    QDEBUGVAR(backupPath + "/Contents/Resources/app.nw/" + gameHack.target);
                    otherFiles[otherFilesPath] = readFile(backupPath + "/Contents/Resources/app.nw/" + gameHack.target);
                }

#else
                    //read from the backup once
                if(otherFiles[otherFilesPath].isEmpty()) {
                    otherFiles[otherFilesPath] = readFile(otherFilesPath);
                }

#endif
                    QDEBUGVAR(otherFiles[otherFilesPath].size());

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
        }

        if((!QFile::exists(unpackPath + "/package.nw/package.json") && !QFile::exists(unpackPath + "/package.nw/index.html"))) {


            QFile packageJSON(unpackPath + "/package.json");
            QFile indexHTML(unpackPath + "/" + xml.launchfile);

            if(packageJSON.open(QIODevice::WriteOnly)) {
                packageJSON.write(packageJSONString.toLatin1());
                packageJSON.close();
            } else {
                QDEBUG() << "failed to open packageJSON";
                return false;
            }


            if(indexHTML.open(QIODevice::WriteOnly)) {
                indexHTML.write(indexHTMLString.toLatin1());
                indexHTML.close();
            } else {
                QDEBUG() << "failed to open indexHTML";
                return false;
            }

        }


        QDEBUGVAR(backupPath);
        QDEBUGVAR(packPath);

        if(!cpDir(backupPath, packPath)) {
            QDEBUG() <<"Could not recursive copy" << backupPath <<"to" << packPath;
            return false;
        }
        if(xml.app_nw_is_compressed) {

            JlCompress::compressDir(packPath + "/app.nw", unpackPath);
            if(xml.app_nw_is_hidden) {

                QByteArray appNWByteArrayFinal = readFile(packPath + "/app.nw");
                deletefile(packPath + "/" + xml.target);
                deletefile(packPath + "/app.nw");

                QFile finalTarget(packPath + "/"+xml.outputname);

                if(finalTarget.open(QIODevice::WriteOnly)) {
                    finalTarget.write(nwEXEByteArray);
                    finalTarget.write(appNWByteArrayFinal);
                    finalTarget.close();
                } else {
                    QDEBUG() << "failed to open indexHTML";
                    return false;
                }
            } else {
                //moved packed file to the correct place.
                deletefile(packPath + "/" + xml.target);
                QFile::rename(packPath + "/app.nw", packPath + "/" + xml.target);
            }
        } else {

            //No compression. Just need to copy over modded files.
            QDir nwpath;

#ifdef __APPLE__
            rmDir(packPath + "/Contents/Resources/app.nw/");
            nwpath.mkpath(packPath + "/Contents/Resources/app.nw/");
            cpDir(unpackPath, packPath + "/Contents/Resources/app.nw/");
#else
            rmDir(packPath);
            nwpath.mkpath(packPath);
            cpDir(unpackPath, packPath);

#endif

        }



        deletefile(packPath + "/index.html.backup");
        deletefile(packPath + "/package.json.backup");

        return true;
    } else {
        QDEBUG() << "I am not unpacked. I cannot do this";

    }

    return false;
}

