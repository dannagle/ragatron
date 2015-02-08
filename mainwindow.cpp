/*
This file is part of Ragatron.
It is (c) Dan Nagle http://DanNagle.com/
It is licensed GPL v2 or later.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <QTime>
#include <QDate>


#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"



#define SETTINGSCHECK(var) settings.setValue( # var , ui-> var ->isChecked())
#define GETSETTINGSCHECK(var) ui-> var ->setChecked(settings.value(# var).toBool())


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->menuBar->hide();
    ui->mainToolBar->hide();


    QSettings settings(SETTINGSFILE, QSettings::IniFormat);
    QString unpackPathEdt = settings.value("unpackPathEdt",QDir::homePath() + "/Downloads/ragatron/").toString();
    ui->unpackPathEdt->setText(unpackPathEdt);

    if(!QFile::exists(unpackPathEdt)) {
        QDir().mkpath(unpackPathEdt);
    }

    WizardsLizard.pathString = WIZARDSLIZARD_EXE_BACKUP;
    WizardsLizard.exeBYTELocation = NW_EXE_LOCATION_AWL;
    WizardsLizard.gameMD5 = WIZARDLIZARD_MD5;
    WizardsLizard.gameFileList << WIZARDSLIZARD_EXE_BACKUP
                              << FFMPEGSUMO_DLL_BACKUP
                              << ICUDT_DLL_BACKUP
                              << LIBEGL_DLL_BACKUP
                              << LIBGLESV2_DLL_BACKUP
                              << NW_PAK_BACKUP;

    WizardsLizard.repackName = "WizardsLizardHacked.exe";
    WizardsLizard.exeNameOriginal = "wizardslizard.exe";
    WizardsLizard.appNW = APP_NW;
    WizardsLizard.nwEXE = NW_EXE;
    WizardsLizard.packPath = unpackPathEdt;
    WizardsLizard.packPath.append(QString("/"));
    WizardsLizard.packPath.append("wizardslizard");
    WizardsLizard.indexHTMLBackup = INDEX_HTML_BACKUP;
    WizardsLizard.packageJSONBackup = PACKAGE_JSON_BACKUP;


    LavaBlade.pathString = LAVABLADE_EXE_BACKUP;
    LavaBlade.exeBYTELocation = NW_EXE_LOCATION_LAVABLADE;
    LavaBlade.gameMD5 = LAVABLADE_MD5;
    LavaBlade.gameFileList << LAVABLADE_EXE_BACKUP
                           << ICUDT_DLL_BACKUP_LAVABLADE
                           << FFMPEGSUMO_DLL_BACKUP_LAVABLADE
                           << NW_PAK_BACKUP_LAVABLADE;

    LavaBlade.repackName = "LavaBladeHacked.exe";
    LavaBlade.exeNameOriginal = "lavablade.exe";
    LavaBlade.appNW = APP_NW_LAVABLADE;
    LavaBlade.nwEXE = NW_EXE_LAVABLADE;
    LavaBlade.packPath = unpackPathEdt;
    LavaBlade.packPath.append(QString("/"));
    LavaBlade.packPath.append("lavablade");
    LavaBlade.indexHTMLBackup = INDEX_HTML_BACKUP_LAVABLADE;
    LavaBlade.packageJSONBackup = PACKAGE_JSON_BACKUP_LAVABLADE;


    statusBar()->show();

    QDate vDate = QDate::fromString(QString(__DATE__).simplified(), "MMM d yyyy");
    statusBarMessage("Version " + vDate.toString("yyyy-MM-dd"));


    setWindowTitle("Ragatron: Modding Lost Decade Games");
    setWindowIcon(QIcon(RAGATRONLOGO));


    ui->wizardlizardPathEdit->setReadOnly(true);
    ui->lavaBladePathEdit->setReadOnly(true);
    ui->unpackPathEdt->setReadOnly(true);
    ui->unpackButton->setEnabled(false);
    ui->unpackPathButton->setEnabled(false);
    ui->unpackLavaBladeButton->setEnabled(false);
    ui->repackButton->setEnabled(false);
    ui->repackButtonLavaBlade->setEnabled(false);

    ui->awlTabWidget->setCurrentIndex(0);
    ui->lavabladeTabWidget->setCurrentIndex(0);
    ui->gameChoiceTabs->setCurrentIndex(0);


    successfulUnpackWL = settings.value("successfulUnpackWL", false).toBool();
    successfulUnpackLB =settings.value("successfulUnpackLB", false).toBool();;

    QPushButton * RagatronButton = new QPushButton("Ragatron.com");
    RagatronButton->setStyleSheet("QPushButton { color: black; } QPushButton::hover { color: #BC810C; } ");
    RagatronButton->setFlat(true);
    RagatronButton->setCursor(Qt::PointingHandCursor);
    RagatronButton->setIcon(QIcon(RAGATRONLOGO));

    connect(RagatronButton, SIGNAL(clicked()),
            this, SLOT(gotoRagatron()));

    statusBar()->insertPermanentWidget(0, RagatronButton);


    QDir mdir;
    mdir.mkpath(TEMPPATH);
    mdir.mkpath(SETTINGSPATH);

    GETSETTINGSCHECK(fastSoulOrbCheck);
    GETSETTINGSCHECK(fastDashCheck);
    GETSETTINGSCHECK(fastTotemCheck);
    GETSETTINGSCHECK(startwith1millionCheck);
    GETSETTINGSCHECK(startwith5000orbsCheck);
    GETSETTINGSCHECK(fastSkyrocketCheck);
    GETSETTINGSCHECK(quadSpearCheck);
    GETSETTINGSCHECK(start5000healthCheck);
    GETSETTINGSCHECK(enableDeveloperCheck);

    //Lavablade
    GETSETTINGSCHECK(enableLavaBladeDeveloperCheck);
    GETSETTINGSCHECK(lavablade_cheapHealCheck);
    GETSETTINGSCHECK(lavabladeLarsMovementCheck);
    GETSETTINGSCHECK(lavabladeLunaAttackCheck);
    GETSETTINGSCHECK(lavabladeSeraphExtraHealthCheck);
    GETSETTINGSCHECK(lavabladeSobekMagicCheck);
    GETSETTINGSCHECK(lavabladeEarnMoreGoldCheck);
    GETSETTINGSCHECK(lavabladeBlitzExtraAllCheck);


    hyperLinkStyle = "QPushButton { color: blue;qlineargradient(spread:pad, x1:0, y1:0.54, x2:1, y2:0, stop:0 rgba(0, 111, 0, 255), stop:1 rgba(255, 255, 255, 255)); } QPushButton::hover { color: #BC810C; } ";
    validChoiceStyle = "QLineEdit { background-color: #AAEE78; }";


    if(WizardsLizard.hasBackup() && WizardsLizard.isEXEValid()) {
        ui->unpackPathButton->setEnabled(true);
        ui->wizardlizardPathEdit->setStyleSheet(validChoiceStyle);
        ui->wizardLizardLabel->setText("AWL Backup Found!");
        ui->wizardLizardLabel->setStyleSheet("QLabel { background-color: #AAEE78; }");
        ui->wizardlizardPathEdit->hide();
        ui->wizardpathButton->hide();

    }


    if(LavaBlade.hasBackup() && LavaBlade.isEXEValid()) {
        ui->unpackLavaBladeButton->setEnabled(true);
        ui->lavaBladePathEdit->setStyleSheet(validChoiceStyle);
        ui->lavaBladeLabel->setText("Lava Blade Backup Found!");
        ui->lavaBladeLabel->setStyleSheet("QLabel { background-color: #AAEE78; }");
        ui->lavaBladePathEdit->hide();
        ui->lavabladepathButton->hide();

    }

    if(!unpackPathEdt.isEmpty() && QFile::exists(unpackPathEdt)) {
        ui->unpackPathEdt->setText(unpackPathEdt);
        ui->unpackPathEdt->setStyleSheet(validChoiceStyle);
        if(WizardsLizard.canUnpack()) {
            ui->unpackPathEdt->setStyleSheet(validChoiceStyle);
            ui->unpackButton->setEnabled(true);
        }
        if(LavaBlade.canUnpack()) {
            ui->lavaBladePathEdit->setStyleSheet(validChoiceStyle);
            ui->unpackLavaBladeButton->setEnabled(true);
        }
    }

    if(successfulUnpackWL) {
        ui->repackButton->setEnabled(true);
    }
    if(successfulUnpackLB) {
        ui->repackButtonLavaBlade->setEnabled(true);
    }

}

void MainWindow::statusBarMessage(const QString &msg, int timeout, bool override )
{
    QString currentMsg = statusBar()->currentMessage();

    if(currentMsg.size() > 10)
    {
        override = true;
    }
    if(currentMsg.size() > 0 && !override)
    {
        statusBar()->showMessage(currentMsg + " / " + msg, timeout * 2);
    } else {
        if (timeout == 0)
            timeout = 3000;
        statusBar()->showMessage(msg, timeout);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::byteArrayToHex(QByteArray data)
{
    QString byte, returnString;
  //  QDEBUG() << "size is " <<data.size();

    returnString.clear();
    if(data.isEmpty())
    {
        return "";
    }

    for(int i = 0; i < data.size(); i++)
    {
        byte = QString::number((unsigned char)data.at(i) & 0xff, 16);

        if(byte.size() % 2 == 1)
        {
            byte.prepend("0");
        }
        returnString.append(byte);
        returnString.append(" ");
    }

    return returnString.trimmed().toUpper();

}



void MainWindow::gotoRagatron()
{
    //Open URL in browser
    QDesktopServices::openUrl(QUrl("http://ragatron.com/"));

}


void MainWindow::gotoDanNagleTwitter()
{

    //Open URL in browser
    QDesktopServices::openUrl(QUrl("https://twitter.com/NagleCode"));

}

QPushButton *MainWindow::generateDNLink()
{

    QPushButton * returnButton = new QPushButton("DanNagle.com");
    returnButton->setStyleSheet(hyperLinkStyle);
    returnButton->setIcon( QIcon(":dannagle_logo.png"));
    returnButton->setFlat(true);
    returnButton->setCursor(Qt::PointingHandCursor);
    connect(returnButton, SIGNAL(clicked()),
            this, SLOT(gotoDanNagleTwitter()));

    return returnButton;

}

void MainWindow::on_wizardpathButton_clicked()
{

    QString defaultpath = qgetenv("programfiles");
    QString defaultpathTest = defaultpath;
    defaultpathTest.append("/").append("LostDecadeGames");

    QFile test(defaultpathTest);
    if(test.exists()) {
        test.close();
        defaultpath = defaultpathTest;
    }


//Mac does not work yet.

#ifdef Q_OS_MAC
    QString fileName =  "/Applications/A Wizard's Lizard.app/Contents/Resources/app.nw";

    QStringList zipFileList = JlCompress::getFileList(fileName);
    QDEBUGVAR(zipFileList);

#else
    QString fileName =  QFileDialog::getExistingDirectory(this, tr("A Wizards Lizard Directory"),
                                                      defaultpath,
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);;

#endif







    QDEBUGVAR(fileName);
    if(!fileName.isEmpty()) {
        ui->wizardlizardPathEdit->setText(fileName);
#ifdef Q_OS_MAC
        WizardsLizard.pathString = fileName;

#else
        WizardsLizard.pathString = fileName + "/" + "wizardslizard.exe";

#endif
        if(WizardsLizard.isEXEValid()) {
            QDEBUG() << "valid exe";
            statusBarMessage("valid exe");
            ui->unpackPathButton->setEnabled(true);
            ui->wizardlizardPathEdit->setStyleSheet(validChoiceStyle);
            if(WizardsLizard.saveBackup(fileName)) {
                QDEBUG() << "Successfully made a copy";
                statusBarMessage("Successfully made a copy");
                ui->wizardLizardLabel->setText("Got it! Made a backup.");
                ui->wizardLizardLabel->setStyleSheet("QLabel { background-color: #AAEE78; }");
                ui->wizardlizardPathEdit->hide();
                ui->wizardpathButton->hide();

            } else {
                QMessageBox::warning(this, "No backup.", "Attempts to make a backup have failed.");
            }
        } else {
            ui->wizardlizardPathEdit->setStyleSheet("");
            QMessageBox::warning(this, "Unknown EXE", "I do not recognize this file:\n\n" + fileName);
            WizardsLizard.gameByteArray.clear();
            WizardsLizard.nwEXEByteArray.clear();
            WizardsLizard.appNWByteArray.clear();
        }
    }

    if(WizardsLizard.canUnpack()) {
        ui->unpackButton->setEnabled(true);
    }


}

void MainWindow::on_unpackPathButton_clicked()
{

    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    QString path =  settings.value("unpackPathEdt",
                                   QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)


                                   ).toString();


    path =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      path,
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);;
    QDEBUGVAR(path);

    if(path.isEmpty()) {
        return;
    }

    int filecount = QDir(path).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count();

    QDEBUG() << path << filecount;
    if(filecount > 0)
    {


        QMessageBox msgBox;
        msgBox.setWindowTitle("Files detected!");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Found " + QString::number(filecount) + " files! You must choose an empty directory. \n\nAdd \"ragatron\" to path?");
        int yesno = msgBox.exec();
        if(yesno == QMessageBox::Yes) {

            path.append("/ragatron/");
            QDir mdir;
            mdir.mkpath(path);
//            return;

        } else {
            return;
        }



    }

    if(!path.contains("ragatron")) {
        path.append("/ragatron/");
    }
    QDir mdir;
    QString pathWizardsLizard = path;
    QString pathWizardLavaBlade = path;
    pathWizardsLizard.append("wizardslizard/");
    pathWizardLavaBlade.append("lavablade/");

    mdir.mkpath(pathWizardsLizard);
    mdir.mkpath(pathWizardLavaBlade);

    QDir directoryTest;
    directoryTest.setPath(path);
    if (directoryTest.exists()){
        ui->unpackPathEdt->setText(directoryTest.canonicalPath());
        ui->unpackButton->setEnabled(true);
        ui->unpackLavaBladeButton->setEnabled(true);
        ui->unpackPathEdt->setStyleSheet(validChoiceStyle);


        settings.setValue("unpackPathEdt", directoryTest.canonicalPath());
        statusBarMessage("Unpack finished:" +path );
    } else {

        QMessageBox::warning(this, "Unwritable",
                             "I cannot write to this location:\n\n" + directoryTest.canonicalPath());

        ui->unpackPathEdt->setText("");
        ui->unpackButton->setEnabled(false);
        ui->unpackLavaBladeButton->setEnabled(false);
        ui->unpackPathEdt->setStyleSheet("");
    }




}

void MainWindow::on_unpackButton_clicked()
{
    if(WizardsLizard.canUnpack()) {
        QDEBUG() <<"Yes, we can unpack.";
        QString unpackDir = ui->unpackPathEdt->text();
        WizardsLizard.packPath = unpackDir;
        WizardsLizard.packPath.append(QString("/"));
        WizardsLizard.packPath.append("wizardslizard");

        int filecount = QDir(unpackDir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count();
        QDEBUG() << unpackDir << filecount;
        if(filecount > 0)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Files detected!");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("This will overwrite any mods you made with original data. Continue?");
            int yesno = msgBox.exec();
            if(yesno == QMessageBox::No) {
                return;
            }
        }

        WizardsLizard.extractEXE();
        QDesktopServices::openUrl( ui->unpackPathEdt->text());
        ui->repackButton->setEnabled(true);

        QSettings settings(SETTINGSFILE, QSettings::IniFormat);
        settings.setValue("successfulUnpackWL", true);

    } else {
        QDEBUG() <<"Cannot unpack";
    }
}


bool MainWindow::copySupportFiles(QString path, HTML5Game * h5game) {

    QStringList testFiles = h5game->gameFileList;
    QString testFile;
    QFileInfo pathTest(path);
    QString dirPath = pathTest.absoluteDir().canonicalPath();
    dirPath.append(QString("/"));
    QDir dirPathDir;
    dirPathDir.mkpath(dirPath);
    if(pathTest.absoluteDir().exists()) {

        foreach(testFile, testFiles) {
            if(testFile == h5game->gameFileList[0]) {
                continue;
            }
            QFileInfo testInfo(testFile);
            QString fileName = testInfo.fileName();
            QFile::copy(testFile, dirPath +  fileName);

        }
    }

    return true;

}

void MainWindow::on_repackButton_clicked()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    ui->repackButton->setEnabled(false);
    QString fileName = ui->unpackPathEdt->text();
    fileName.append(QString("/"));
    fileName.append("wizardslizard");
    WizardsLizard.packPath = fileName;

    QString packPath = QString(fileName + "_mod"  + "/");
    QDir().rmpath(packPath);
    QDir().mkpath(packPath);

    QString indexHTMLpath = WizardsLizard.packPath + "/" + "index.html";
    QString packageJSONpath =WizardsLizard.packPath + "/" + "package.json";
    QDir dirPath(packPath);
    dirPath.rmpath(packPath);
    dirPath.mkpath(packPath);

    QDEBUGVAR(indexHTMLpath);
    fileName = WizardsLizard.packPath +  WizardsLizard.repackName;;
    QDEBUGVAR(fileName);
    QFile::remove(fileName);
    QString programfiles = qgetenv("programfiles").toLower();

    if(fileName.toLower().contains(programfiles)) {
        QDEBUG() <<"Attempting to save to program files";

    }


    if(!fileName.isEmpty()) {

        QFile::remove(indexHTMLpath);
        QFile::copy(WizardsLizard.indexHTMLBackup, indexHTMLpath);
        QFile::remove(packageJSONpath);
        QFile::copy(WizardsLizard.packageJSONBackup, packageJSONpath);

        copySupportFiles(packPath, &WizardsLizard);


        QFile fileIndex(indexHTMLpath);

        WizardsLizard.indexHTMLByteArray.clear();
        if (fileIndex.open(QIODevice::ReadOnly)) {
            WizardsLizard.indexHTMLByteArray = fileIndex.readAll();
            QDEBUG() << "Read" << WizardsLizard.indexHTMLByteArray.size() << "bytes";
            fileIndex.close();
        }


        SETTINGSCHECK(enableDeveloperCheck);
        if(ui->enableDeveloperCheck->isChecked()) {

            QFile fileJSONIndex(WizardsLizard.packageJSONBackup);
            if (fileJSONIndex.open(QIODevice::ReadOnly)) {
                WizardsLizard.packageJSONByteArray = fileJSONIndex.readAll();
                QDEBUG() << "Read" << WizardsLizard.packageJSONByteArray.size() << "bytes";
                fileJSONIndex.close();
                QString jsonString(WizardsLizard.packageJSONByteArray);
                fileJSONIndex.setFileName(packageJSONpath);
                jsonString.replace("\"toolbar\": false","\"toolbar\": true");
                if (fileJSONIndex.open(QIODevice::WriteOnly)) {
                     fileJSONIndex.write(jsonString.toLatin1());
                     fileJSONIndex.close();
                }
            }
        }


        QString indexString(WizardsLizard.indexHTMLByteArray);
        indexString.replace("<title>A Wizard&#39;s Lizard</title>",
                            "<title>A Wizard&#39;s Lizard - Ragatron Edition</title>");

        SETTINGSCHECK(fastSoulOrbCheck);
        if(ui->fastSoulOrbCheck->isChecked()) {
            indexString.replace("soulCooldown:5e3", "soulCooldown:500");
        }


        SETTINGSCHECK(start5000healthCheck);
        if(ui->start5000healthCheck->isChecked()) {
            indexString.replace("{groups:[\"player\"],targetGroups:[\"collectible\"],solidGroups:[\"solid\",\"mortalSolid\",\"lightSolid\"]},mortal:{bleeds:\"bloodPuddle\",healthMax:50,graceDuration",
                                "{groups:[\"player\"],targetGroups:[\"collectible\"],solidGroups:[\"solid\",\"mortalSolid\",\"lightSolid\"]},mortal:{bleeds:\"bloodPuddle\",healthMax:5000,graceDuration");
        }



        SETTINGSCHECK(fastDashCheck);
        if(ui->fastDashCheck->isChecked()) {
            indexString.replace("dashCooldown:5e3", "dashCooldown:500");
        }
        SETTINGSCHECK(fastTotemCheck);
        if(ui->fastTotemCheck->isChecked()) {
            indexString.replace("Distance);var o=2e4-1e3*r.totemPower", "Distance);var o=2e3-1e3*r.totemPower");
        }
        SETTINGSCHECK(startwith1millionCheck);
        if(ui->startwith1millionCheck->isChecked()) {
            indexString.replace("getStartingGold:function(){var e=this.get(\"hostages\",0);return p+e*p}",
                                "getStartingGold:function(){var e=this.get(\"hostages\",0);return 1000000+e*p}");
        }
        SETTINGSCHECK(startwith5000orbsCheck);
        if(ui->startwith5000orbsCheck->isChecked()) {
            indexString.replace("items:{soulOrb:3}},stats:{}",
                                "items:{soulOrb:5000}},stats:{}");
        }
        SETTINGSCHECK(fastSkyrocketCheck);
        if(ui->fastSkyrocketCheck->isChecked()) {
            indexString.replace("\"skyrocket\",effects:{modifyStats:{attackCooldown:450}}}",
                                "\"skyrocket\",effects:{modifyStats:{attackCooldown:50}}}");
        }
        SETTINGSCHECK(quadSpearCheck);
        if(ui->quadSpearCheck->isChecked()) {
            indexString.replace("spear:{status:1,intlKey:\"ITEM_SPEAR\",costBase:2500,slot:\"weapon\",prefabType:\"spear\"",
                                "spear:{status:1,intlKey:\"ITEM_SPEAR\",costBase:2500,slot:\"weapon\",prefabType:\"spear\",effects:{modifyStats:{projectileCount:3,attackCooldown:-150}}");
        }

        if (fileIndex.open(QIODevice::WriteOnly)) {
            WizardsLizard.indexHTMLByteArray = indexString.toLatin1();
            fileIndex.write(WizardsLizard.indexHTMLByteArray);
            fileIndex.close();
        }




        QDEBUG();


        QString fileName = packPath + WizardsLizard.repackName;
        QDEBUGVAR(fileName);
        QDEBUGVAR(WizardsLizard.packPath);


#ifdef Q_OS_WIN32
        JlCompress::compressDir(fileName, WizardsLizard.packPath);
#endif
        QDEBUG();
        QFile indexHTMLFile(indexHTMLpath);
        QDEBUG();
        indexHTMLFile.remove();
        QDEBUG();
        QFile packageJSONFile(packageJSONpath);
        QDEBUG();
        packageJSONFile.remove();
        QDEBUG();


        QFile file(fileName);
        QDEBUG();
        if (file.open(QIODevice::ReadOnly)) {
            QDEBUG();
            QByteArray appNW = file.readAll();
            QDEBUG() << "Read" << appNW.size() << "bytes";
            file.close();
            QDEBUG();

            if (file.open(QIODevice::WriteOnly)) {
                file.write(WizardsLizard.nwEXEByteArray);
                file.write(appNW);
                file.close();

                QFileInfo fileInfo(file.fileName());

                QMessageBox msgBox;
                msgBox.setWindowTitle("Finished!");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText("Launch game?");
                int yesno = msgBox.exec();
                if(yesno == QMessageBox::Yes) {

                    QDesktopServices::openUrl( fileInfo.canonicalFilePath());

                }

            } else {

                QDEBUG();

                QMessageBox::warning(this, "Unwritable",
                                     "I cannot write to this location:\n\n" +
                                     fileName + "\n\nIf you are trying to save to a protected directory (like \"Program Files\"), running me as Administrator might help.\n\n" \
                                     "Right-click -> \"Run as Administrator\"");

            }
        }

    }
    QDEBUG();

    statusBarMessage("Finished.");
    ui->repackButton->setEnabled(true);
    QDEBUG();


}

void MainWindow::on_launchUnpackFolderButton_clicked()
{

    QDesktopServices::openUrl(ui->unpackPathEdt->text());
}

void MainWindow::on_lavabladepathButton_clicked()
{

    QString defaultpath = qgetenv("programfiles");
    QString defaultpathTest = defaultpath;
    defaultpathTest.append("/").append("LostDecadeGames");
    QFile test(defaultpathTest);
    if(test.exists()) {
        test.close();
        defaultpath = defaultpathTest;
    }

#ifdef Q_OS_MAC
    defaultpath = "/Applications/";
#endif


    QString fileName =  QFileDialog::getExistingDirectory(this, tr("Lava Blade Directory"),
                                                      defaultpath,
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);;



    QDEBUGVAR(fileName);
    if(!fileName.isEmpty()) {
        ui->lavaBladePathEdit->setText(fileName);
        LavaBlade.pathString = fileName + "/" + "lavablade.exe";
        if(LavaBlade.isEXEValid()) {
            LavaBlade.pathString = LAVABLADE_EXE_BACKUP;
            QDEBUG() << "valid exe";
            statusBarMessage("valid exe");
            ui->unpackPathButton->setEnabled(true);
            ui->lavaBladePathEdit->setStyleSheet(validChoiceStyle);
            if(LavaBlade.saveBackup(fileName)) {
                QDEBUG() << "Successfully made a copy";
                statusBarMessage("Successfully made a copy");
                ui->lavaBladeLabel->setText("Got it! Made a backup.");
                ui->lavaBladeLabel->setStyleSheet("QLabel { background-color: #AAEE78; }");
                ui->lavaBladePathEdit->hide();
                ui->lavabladepathButton->hide();
            } else {
                QMessageBox::warning(this, "No backup.", "Attempts to make a backup have failed.");
            }
        } else {
            ui->lavaBladePathEdit->setStyleSheet("");
            QMessageBox::warning(this, "Unknown EXE", "I do not recognize this file:\n\n" + fileName);
            LavaBlade.gameByteArray.clear();
            LavaBlade.nwEXEByteArray.clear();
            LavaBlade.appNWByteArray.clear();
        }
    }

    if(LavaBlade.canUnpack()) {
        ui->unpackLavaBladeButton->setEnabled(true);
    }

}

void MainWindow::on_repackButtonLavaBlade_clicked()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    ui->repackButtonLavaBlade->setEnabled(false);
    QString fileName = ui->unpackPathEdt->text();
    fileName.append("lavablade");
    LavaBlade.packPath = fileName;

    QString packPath = QString(fileName + "_mod"  + "/");
    QDir().rmpath(packPath);
    QDir().mkpath(packPath);

    QString indexHTMLpath = LavaBlade.packPath + "/" + "index.html";
    QString packageJSONpath =LavaBlade.packPath + "/" + "package.json";
    QDir dirPath(packPath);
    dirPath.rmpath(packPath);
    dirPath.mkpath(packPath);

    QDEBUGVAR(indexHTMLpath);
    fileName = LavaBlade.packPath +  LavaBlade.repackName;;
    QDEBUGVAR(fileName);
    QFile::remove(fileName);
    QString programfiles = qgetenv("programfiles").toLower();

    if(fileName.toLower().contains(programfiles)) {
        QDEBUG() <<"Attempting to save to program files";

    }
    if(!fileName.isEmpty()) {

        QFile::remove(indexHTMLpath);
        QFile::copy(LavaBlade.indexHTMLBackup, indexHTMLpath);
        QFile::remove(packageJSONpath);
        QFile::copy(LavaBlade.packageJSONBackup, packageJSONpath);

        copySupportFiles(packPath, &LavaBlade);


        QFile fileIndex(indexHTMLpath);

        LavaBlade.indexHTMLByteArray.clear();
        if (fileIndex.open(QIODevice::ReadOnly)) {
            LavaBlade.indexHTMLByteArray = fileIndex.readAll();
            QDEBUG() << "Read" << LavaBlade.indexHTMLByteArray.size() << "bytes";
            fileIndex.close();
        }


        SETTINGSCHECK(enableLavaBladeDeveloperCheck);
        if(ui->enableLavaBladeDeveloperCheck->isChecked()) {

            QFile fileJSONIndex(LavaBlade.packageJSONBackup);
            if (fileJSONIndex.open(QIODevice::ReadOnly)) {
                LavaBlade.packageJSONByteArray = fileJSONIndex.readAll();
                QDEBUG() << "Read" << LavaBlade.packageJSONByteArray.size() << "bytes";
                fileJSONIndex.close();
                QString jsonString(LavaBlade.packageJSONByteArray);
                fileJSONIndex.setFileName(packageJSONpath);
                jsonString.replace("\"toolbar\": false","\"toolbar\": true");
                if (fileJSONIndex.open(QIODevice::WriteOnly)) {
                     fileJSONIndex.write(jsonString.toLatin1());
                     fileJSONIndex.close();
                }
            } else {
                QDEBUG() <<"Could not open" << LavaBlade.packageJSONBackup;
            }
        }


        QString indexString(LavaBlade.indexHTMLByteArray);
        indexString.replace("<title>Lava Blade</title>",
                            "<title>Lava Blade - Ragatron Edition</title>");

        SETTINGSCHECK(lavablade_cheapHealCheck);
        if(ui->lavablade_cheapHealCheck->isChecked()) {
            indexString.replace("getHealCost:function(){return Math.max(~~(this.data.gold*.1),50)}",
                                "getHealCost:function(){return 1}");
        }

        SETTINGSCHECK(lavabladeLarsMovementCheck);
        if(ui->lavabladeLarsMovementCheck->isChecked()) {
            indexString.replace("{warrior:{conf:{behavior:\"player\",healthMax:100,attackPower:20,movement:2,initiative:2}}",
                                "{warrior:{conf:{behavior:\"player\",healthMax:100,attackPower:20,movement:8,initiative:5}}");
        }

        SETTINGSCHECK(lavabladeLunaAttackCheck);
        if(ui->lavabladeLunaAttackCheck->isChecked()) {
            indexString.replace("amazon:{conf:{behavior:\"player\",healthMax:100,attackPower:20,movement:2,initiative:2}}",
                                "amazon:{conf:{behavior:\"player\",healthMax:100,attackPower:150,movement:2,initiative:2}}");
        }

        SETTINGSCHECK(lavabladeSeraphExtraHealthCheck);
        if(ui->lavabladeSeraphExtraHealthCheck->isChecked()) {
            indexString.replace("monk:{conf:{behavior:\"player\",healthMax:100,magicPower:20,energyMax:50,movement:2,initiative:1}}",
                                "monk:{conf:{behavior:\"player\",healthMax:500,magicPower:20,energyMax:50,movement:2,initiative:1}}");
        }
        SETTINGSCHECK(lavabladeSobekMagicCheck);
        if(ui->lavabladeSobekMagicCheck->isChecked()) {
            indexString.replace("sobek:{conf:{behavior:\"player\",healthMax:100,magicPower:20,energyMax:50,movement:2,initiative:1}}",
                                "sobek:{conf:{behavior:\"player\",healthMax:100,magicPower:70,energyMax:500,movement:2,initiative:1}}");
        }

        SETTINGSCHECK(lavabladeBlitzExtraAllCheck);
        if(ui->lavabladeBlitzExtraAllCheck->isChecked()) {
            indexString.replace("ninja:{conf:{behavior:\"player\",healthMax:140,attackPower:20,movement:2,initiative:3}}",
                                "ninja:{conf:{behavior:\"player\",healthMax:340,attackPower:70,movement:8,initiative:8}}");
        }

        //

        SETTINGSCHECK(lavabladeEarnMoreGoldCheck);
        if(ui->lavabladeEarnMoreGoldCheck->isChecked()) {
            indexString.replace("goldBountyBase:",
                                "goldBountyBase:1");
        }



        if (fileIndex.open(QIODevice::WriteOnly)) {
            LavaBlade.indexHTMLByteArray = indexString.toLatin1();
            fileIndex.write(LavaBlade.indexHTMLByteArray);
            fileIndex.close();
        }




        QDEBUG();


        QString fileName = packPath + LavaBlade.repackName;
        QDEBUGVAR(fileName);
        QDEBUGVAR(LavaBlade.packPath);


#ifdef Q_OS_WIN32
        JlCompress::compressDir(fileName, LavaBlade.packPath);
#endif
        QDEBUG();
        QFile indexHTMLFile(indexHTMLpath);
        QDEBUG();
        indexHTMLFile.remove();
        QDEBUG();
        QFile packageJSONFile(packageJSONpath);
        QDEBUG();
        packageJSONFile.remove();
        QDEBUG();


        QFile file(fileName);
        QDEBUG();
        if (file.open(QIODevice::ReadOnly)) {
            QDEBUG();
            QByteArray appNW = file.readAll();
            QDEBUG() << "Read" << appNW.size() << "bytes";
            file.close();
            QDEBUG();

            if (file.open(QIODevice::WriteOnly)) {
                QDEBUG() <<"Wrote" << file.write(LavaBlade.nwEXEByteArray);
                QDEBUG() <<"Wrote" << file.write(appNW);
                file.close();

                QFileInfo fileInfo(file.fileName());

                QMessageBox msgBox;
                msgBox.setWindowTitle("Finished!");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText("Launch game?");
                int yesno = msgBox.exec();
                if(yesno == QMessageBox::Yes) {

                    QDEBUGVAR(fileInfo.canonicalFilePath());
                    QDesktopServices::openUrl( fileInfo.canonicalFilePath());

                }

            } else {

                QDEBUG();

                QMessageBox::warning(this, "Unwritable",
                                     "I cannot write to this location:\n\n" +
                                     fileName + "\n\nIf you are trying to save to a protected directory (like \"Program Files\"), running me as Administrator might help.\n\n" \
                                     "Right-click -> \"Run as Administrator\"");

            }
        }

    }
    QDEBUG();

    statusBarMessage("Finished.");
    ui->repackButtonLavaBlade->setEnabled(true);
    QDEBUG();



}

void MainWindow::on_unpackLavaBladeButton_clicked()
{
    if(LavaBlade.canUnpack()) {
        QDEBUG() <<"Yes, we can unpack.";
        QString unpackDir = ui->unpackPathEdt->text();
        LavaBlade.packPath = unpackDir;
        LavaBlade.packPath.append(QString("/"));
        LavaBlade.packPath.append("lavablade");
        unpackDir = LavaBlade.packPath;
        QDir().mkpath(unpackDir);

        int filecount = QDir(unpackDir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count();
        QDEBUG() << unpackDir << filecount;
        if(filecount > 0)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Files detected!");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("This will overwrite any mods you made with original data. Continue?");
            int yesno = msgBox.exec();
            if(yesno == QMessageBox::No) {
                return;
            }
        }

        LavaBlade.extractEXE();
        QDesktopServices::openUrl(ui->unpackPathEdt->text());
        ui->repackButtonLavaBlade->setEnabled(true);

        QSettings settings(SETTINGSFILE, QSettings::IniFormat);
        settings.setValue("successfulUnpackLB", true);

    } else {
        QDEBUG() <<"Cannot unpack";
    }
}
