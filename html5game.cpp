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


#include <quazip.h>
#include <quazipfile.h>
#include <JlCompress.h>


#include "html5game.h"

HTML5Game::HTML5Game(QObject *parent) :
    QObject(parent)
{

    pathString.clear();
    gameByteArray.clear();
    packageJSONByteArray.clear();
    indexHTMLByteArray.clear();
    nwEXEByteArray.clear();
    appNWByteArray.clear();
    gameMD5.clear();
    gameFileList.clear();
    zipFileList.clear();
    exeBYTELocation = 0;
    repackName.clear();
    exeNameOriginal.clear();
    packPath.clear();

}


bool HTML5Game::isEXEValid()
{

    unsigned int i = 0;
    if(gameByteArray.isEmpty() || nwEXEByteArray.isEmpty() || appNWByteArray.isEmpty()) {

        QFile file(pathString);
        QDEBUGVAR(pathString);
        if (file.open(QIODevice::ReadOnly)) {
            gameByteArray = file.readAll();
            QDEBUG() << "Read" << gameByteArray.size() << "bytes";
            file.close();

            for(i = 0; i < exeBYTELocation-1; i++) {
                nwEXEByteArray.append(gameByteArray[i]);
            }
            QDEBUGVAR(nwEXEByteArray.size());
            for(i = exeBYTELocation; i < gameByteArray.size(); i++) {
                appNWByteArray.append(gameByteArray[i]);
            }
            QDEBUGVAR(appNWByteArray.size());
        }

    }

    QByteArray fileHash;
    fileHash = QCryptographicHash::hash(gameByteArray, QCryptographicHash::Md5);
    QString MD5 = fileHash.toHex().toLower();
    if(MD5 == gameMD5) {
        return true;
    } else {
        gameByteArray.clear();
        nwEXEByteArray.clear();
        appNWByteArray.clear();
    }

    QDEBUG() << "MD5 was" << MD5 <<"it should be" << gameMD5;
    return false;
}


bool HTML5Game::saveBackup(QString path) {

    QStringList testFiles = gameFileList;
    QString testFile;
    QString dirPath = path;
    QFileInfo pathTest(path);
    if(!pathTest.isDir()) {
        dirPath = pathTest.absoluteDir().canonicalPath() + "/";
    }
    pathTest.setFile(dirPath);
    QDEBUG() << "Saving " << dirPath;
    if(pathTest.exists()) {

        foreach(testFile, testFiles) {
            QFileInfo testInfo(testFile);
            QDir testInfoDir = testInfo.absoluteDir();
            testInfoDir.mkpath(testInfo.absolutePath());
            QString fileName = testInfo.fileName();
            QDEBUG() << "Saving " << (dirPath + "/" +  fileName) << "to" << testFile;
            if(QFile::copy((dirPath + "/" +  fileName), testFile)) {
                QDEBUG() << "Copy failed.";
            }


        }
    }

    return hasBackup();
}


bool HTML5Game::canUnpack() {

    QDEBUG();
    if(isEXEValid()) {
        QDEBUG();

        QDir directoryTest;
        directoryTest.mkpath(packPath);
        directoryTest.setPath(packPath);
        if (directoryTest.exists()){
            QDEBUG();
            return true;
        }
    }
    QDEBUG();

    return false;

}

bool HTML5Game::extractEXE() {


    QDEBUGVAR(gameByteArray.size());
    QDEBUGVAR(nwEXEByteArray.size());
    QDEBUGVAR(appNWByteArray.size());

    if(canUnpack()) {
        QDEBUG();
        QString unpackDir = packPath;
        QString unpackNWexe = nwEXE;
        QString unpackAppNW = appNW;
        QDEBUGVAR(appNW);
        QFile unpackNWexeFile(unpackNWexe);
        QFile unpackAppNWFile(unpackAppNW);
        if(unpackAppNWFile.open(QIODevice::WriteOnly)) {
            unpackAppNWFile.write(appNWByteArray);
            unpackAppNWFile.close();
        } else {
            QDEBUG() << "failed to open" << unpackAppNW;
        }

        if(unpackNWexeFile.open(QIODevice::WriteOnly)) {
            unpackNWexeFile.write(nwEXEByteArray);
            QDEBUGVAR(nwEXEByteArray.size());
            unpackNWexeFile.close();
        } else {
            QDEBUG() << "failed to open" << unpackNWexe;
        }

#ifdef Q_OS_WIN32
        QuaZip zip(unpackAppNW);
        if(zip.open(QuaZip::mdUnzip)) {
            zip.close();
            zipFileList = JlCompress::getFileList(unpackAppNW);
            QString fileList;
            QDEBUGVAR(zipFileList);
            JlCompress::extractFiles(unpackAppNW, zipFileList, unpackDir);
            QString indexHTMLLocation = QString(unpackDir+ "/") + "index.html";
            QString packageJSONLocation = QString(unpackDir+ "/") + "package.json";
            QFile::copy(indexHTMLLocation, indexHTMLBackup);
            QFile::copy(packageJSONLocation, packageJSONBackup);

            QFile::copy(":Ragatron_instructions.txt", QString(unpackDir+ "/") + "Ragatron_instructions.txt");

            QFile indexHTMLFile(indexHTMLLocation);
            indexHTMLFile.remove();
            QFile packageJSONFile(packageJSONLocation);
            packageJSONFile.remove();


            //remove read-only nonsense
            QFile ragatronFile(QString(unpackDir+ "/") + "Ragatron_instructions.txt");
            ragatronFile.setPermissions(QFile::ReadOther | QFile::WriteOther);

            foreach(fileList, zipFileList) {
                ragatronFile.setFileName(QString(unpackDir+ "/") + fileList);
                ragatronFile.setPermissions(QFile::ReadOther | QFile::WriteOther);
            }


        } else {
            QDEBUG() <<"unzip of " << unpackAppNW << "failed";
        }
#endif
        unpackNWexeFile.remove();
        unpackAppNWFile.remove();

        return true;


    } else {
        QDEBUG() << "Cannot unpack";
    }

    return false;
}

bool HTML5Game::hasBackup()
{

    QStringList testFiles = gameFileList;

    QString testFile;

    foreach(testFile, testFiles) {
        QFileInfo test;
        test.setFile(testFile);
        if(!test.exists(testFile)) {
            return false;
        }
    }

    return true;
}
