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
#include <QPixmap>
#include <QBuffer>
#include <QTimer>


#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    int reply = QMessageBox::question(this, "This tool violates Steam's ToS"
         , "This tool violates Steam's ToS. Though Lost Decade Games is willing to ignore it, Steam might not.\n\n\nDo you feel lucky?",
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        QDEBUG() <<"Quitting";
        QTimer::singleShot(0, this, SLOT(close()));
        return;
    }



    ui->setupUi(this);

    ui->menuBar->hide();
    ui->mainToolBar->hide();

    this->statusBar()->setSizeGripEnabled(false);

#ifdef __APPLE__
    this->setFixedSize(this->width(),600);
#else
    this->setFixedSize(this->width(),this->height());
#endif




    //setSizeGripEnabled( false ) ;

    player = new QMediaPlayer(this);

    //not ready yet. Gotta fix ssl problem.
    //QUrl mp3Url("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_120x44dp.png");
    //mp3 = new FileDownloader(mp3Url, this);
    //QDEBUG() <<"connect downloader" << connect(mp3, SIGNAL (downloaded()), this, SLOT (playMP3()));


    QSettings settings(SETTINGSFILE, QSettings::IniFormat);
    QString unpackPathEdt = settings.value("unpackPathEdt",QDir::homePath() + "/Downloads/ragatron/").toString();
    ui->unpackPathEdt->setText(unpackPathEdt);

    if(!QFile::exists(unpackPathEdt)) {
        QDir().mkpath(unpackPathEdt);
    }

    if(QFile::exists(unpackPathEdt)) {
        ui->unpackPathEdt->setStyleSheet(VALIDLINEEDITSTYLE);
    }



    statusBar()->show();

    QDate vDate = QDate::fromString(QString(__DATE__).simplified(), "MMM d yyyy");
    statusBarMessage("Version " + vDate.toString("yyyy-MM-dd"));


    setWindowTitle("Ragatron: Hacking HTML5 Games");

    this->setWindowIcon(QIcon(RAGATRONLOGO));


    ui->unpackPathEdt->setReadOnly(true);

    ui->gameChoiceTabs->setCurrentIndex(0);

    QPushButton * RagatronButton = new QPushButton("Ragatron.com");
    RagatronButton->setStyleSheet("QPushButton { color: black; } QPushButton::hover { color: #BC810C; } ");
    RagatronButton->setFlat(true);
    RagatronButton->setCursor(Qt::PointingHandCursor);
    RagatronButton->setIcon(QIcon(RAGATRONLOGO));

    connect(RagatronButton, SIGNAL(clicked()),
            this, SLOT(gotoRagatron()));

    QPushButton * nagleCodeButton = new QPushButton("@NagleCode");
    nagleCodeButton->setStyleSheet("QPushButton { color: black; } QPushButton::hover { color: #BC810C; } ");
    nagleCodeButton->setFlat(true);
    nagleCodeButton->setCursor(Qt::PointingHandCursor);
    nagleCodeButton->setIcon(QIcon("://Twitter_logo_blue.png"));

    connect(nagleCodeButton, SIGNAL(clicked()),
            this, SLOT(gotoDanNagleTwitter()));



    statusBar()->insertPermanentWidget(0, RagatronButton);
    statusBar()->insertPermanentWidget(1, nagleCodeButton);


    QDir mdir;
    mdir.mkpath(TEMPPATH);
    mdir.mkpath(SETTINGSPATH);


    hyperLinkStyle = "QPushButton { color: blue;qlineargradient(spread:pad, x1:0, y1:0.54, x2:1, y2:0, stop:0 rgba(0, 111, 0, 255), stop:1 rgba(255, 255, 255, 255)); } QPushButton::hover { color: #BC810C; } ";
    validChoiceStyle = VALIDLINEEDITSTYLE;


    QStringList searchXML;
    searchXML.clear();
    searchXML.append("*.xml");


    QDir filePath(QCoreApplication::applicationDirPath());
    QStringList xmlList = filePath.entryList(searchXML,
                                             QDir::Files | QDir::NoSymLinks);

    QDEBUGVAR(xmlList);

    html5GameList.clear();
    HTML5Game * game;

    bool scansuccess = false;

    if(!xmlList.isEmpty()) {
        QString xmlFile;
        foreach(xmlFile, xmlList) {
            QString xmlPath = QCoreApplication::applicationDirPath() + "/" + xmlFile;
            QDEBUGVAR(xmlPath);
            game = new HTML5Game();
            scansuccess = game->scanXML(xmlPath);
            if(scansuccess) {
                html5GameList.append(game);
            }
        }
    } else {
        // load internal list
        HTML5Game * soulthiefHTML5 = new HTML5Game();
        scansuccess = soulthiefHTML5->scanXML(":/cheats/soulthief.xml");
        if(scansuccess) {
            html5GameList.append(soulthiefHTML5);
        }

        // load internal list
        HTML5Game * indiegamesimHTML5 = new HTML5Game();
        scansuccess = indiegamesimHTML5->scanXML(":/cheats/indiegamesim.xml");
        if(scansuccess) {
            html5GameList.append(indiegamesimHTML5);
        }


        HTML5Game * wizardlizardHTML5 = new HTML5Game();
        scansuccess = wizardlizardHTML5->scanXML(":/cheats/wizardlizard.xml");
        if(scansuccess) {
            html5GameList.append(wizardlizardHTML5);
        }

        HTML5Game * lavabladeHTML5 = new HTML5Game();
        html5GameList.append(lavabladeHTML5);
        scansuccess = lavabladeHTML5->scanXML(":/cheats/lavablade.xml");
        if(scansuccess) {
            html5GameList.append(lavabladeHTML5);
        }

        HTML5Game * gamedevTycoonHTML5 = new HTML5Game();
        html5GameList.append(gamedevTycoonHTML5);
        scansuccess = gamedevTycoonHTML5->scanXML(":/cheats/gamedevtycoon.xml");
        if(scansuccess) {
            html5GameList.append(gamedevTycoonHTML5);
        }

        // load internal list
        HTML5Game * elliotQuestHTML5 = new HTML5Game();
        scansuccess = elliotQuestHTML5->scanXML(":/cheats/elliotquest.xml");
        if(scansuccess) {
            html5GameList.append(elliotQuestHTML5);
        }


    }

    if(html5GameList.isEmpty()) {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Game List Empty");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Error! \n\nInvalid XML file inside:\n" + QCoreApplication::applicationDirPath());
        msgBox.exec();

    }

    foreach(game, html5GameList) {

        CheatUI * cheattab = game->cheatTab;
        connect(cheattab, SIGNAL(unpack(xml_t)),
                this, SLOT(html5Unpack(xml_t)));
        connect(cheattab, SIGNAL(repack(xml_t)),
                this, SLOT(html5Pack(xml_t)));

        ui->gameChoiceTabs->addTab(cheattab, game->xml.title);

        game->unpackPath = ui->unpackPathEdt->text() + game->titleClean();


        gameButtonChecks(game);
    }

}

void MainWindow::gameButtonChecks(HTML5Game * game) {


    if(game->isEXEValid()) {
        game->cheatTab->enableUnpack(true);
        QDEBUG() << "I can unpack" << game->titleClean();
    } else {
        game->cheatTab->enableUnpack(false);
        QDEBUG() << "I CANNOT unpack" << game->titleClean();
    }

    if(game->canRepack()) {
        game->cheatTab->enableRepack(true);
        QDEBUG() << "I can repack" << game->titleClean();
    } else {
        game->cheatTab->enableRepack(false);
        QDEBUG() << "I CANNOT repack" << game->titleClean();
    }
}

void MainWindow::html5Unpack(xml_t xml) {

    HTML5Game * game;

    bool found = false;
    foreach(game, html5GameList) {
        if(game->xml.title == xml.title) {
            found = true;
            break;
        }
    }

    if(!found) {
        return;
    }

    QDEBUGVAR(xml.title);
    statusBarMessage("Unpacking " + xml.title);
    game->unpackPath = ui->unpackPathEdt->text() + game->titleClean();
    game->packPath  = game->unpackPath + "_mod";

#ifdef __APPLE__
    game->packPath.append(".app");
#endif

    if(game->unpack()) {
        QDEBUG() << "Unpack success";
        QDesktopServices::openUrl(QUrl("file:///" + game->unpackPath, QUrl::TolerantMode));

    } else {
        QDEBUG() << "Did NOT unpack";
    }

    gameButtonChecks(game);

}

void MainWindow::html5Pack(xml_t xml) {

/*
 * For some reason, Mac is triggering 2 clicks
 * in this function. Can't seem to find it, so I am
 * suppressing it. This does not happen in Windows.
 *
 *
 * */

    static QDateTime lastclick = QDateTime();
    if(lastclick.isValid() && lastclick.addSecs(1) > QDateTime::currentDateTime()) {

        QDEBUG() <<"Too soon";
        return;
    }

    HTML5Game * game;

    bool found = false;
    foreach(game, html5GameList) {
        if(game->xml.title == xml.title) {
            found = true;
            break;
        }
    }

    if(!found) {
        return;
    }
    QDEBUGVAR(xml.title);
    statusBarMessage("Repacking " + xml.title);
    game->unpackPath = ui->unpackPathEdt->text() + game->titleClean();
    game->packPath  = game->unpackPath + "_mod";

#ifdef __APPLE__
    game->packPath.append(".app");
#endif

    QDEBUG() <<"repacking...";

    if(game->repack()) {
        QDEBUG() << "Pack success";

        QFileInfo fileInfo(game->packPath + "/" + game->xml.outputname);
        QMessageBox msgBox;
        msgBox.setWindowTitle("Finished!");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
//        msgBox.setIcon(QMessageBox::Question);
        msgBox.setIconPixmap(QPixmap ("://dannagle_logo_fadewhite.png"));
        msgBox.setText("Success! \n\nLaunch game?");
        int yesno = msgBox.exec();
        if(yesno == QMessageBox::Yes) {

#ifdef __APPLE__
            QString cmd = QString("open %1").arg(game->packPath); // may need QUrl::fromLocalFile(fullpath)

            QDEBUGVAR(cmd);
            QProcess::startDetached(cmd);
#else
            QDEBUGVAR(fileInfo.canonicalFilePath());
            QDesktopServices::openUrl( fileInfo.canonicalFilePath());
#endif


        }

    } else {
        QDEBUG() << "Did NOT pack!";
    }

    lastclick = QDateTime::currentDateTime();

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

void MainWindow::on_launchUnpackFolderButton_clicked()
{

    QDesktopServices::openUrl(QUrl("file:///" + ui->unpackPathEdt->text(), QUrl::TolerantMode));

}


void MainWindow::on_unpackPathButton_clicked()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    const QString downloadsFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    QString path =  settings.value("unpackPathEdt",downloadsFolder).toString();


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
        } else {
            return;
        }
    }

    if(!path.contains("ragatron")) {
        path.append("/ragatron/");
    }
    QDir mdir;
    mdir.mkpath(path);

    ui->unpackPathEdt->setText(path);
    if(QFile(ui->unpackPathEdt->text()).exists()) {
        ui->unpackPathEdt->setStyleSheet(VALIDLINEEDITSTYLE);
    } else {
        ui->unpackPathEdt->setStyleSheet("");
    }

}

void MainWindow::playMP3()
{
    QDEBUG();
    mediaStream = new QBuffer(this);
    QDEBUG();
    mediaStreamBuffer.clear();
    QDEBUG();
    mediaStreamBuffer.append(mp3->downloadedData());
    QDEBUG();
    mediaStream->setBuffer(&mediaStreamBuffer);
    QDEBUG();
    player->setMedia(QMediaContent(), mediaStream);
    QDEBUG();
    player->play();
    QDEBUG();

}

