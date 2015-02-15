#include "cheatui.h"
#include "ui_cheatui.h"

#include <QLabel>
#include <QCheckBox>
#include <QScrollArea>
#include <QSettings>
#include <QDesktopServices>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>


CheatUI::CheatUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheatUI)
{
    ui->setupUi(this);
}

CheatUI::~CheatUI()
{
    delete ui;
}


void CheatUI::generate()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

    hack_t gameHack;

    QStringList categories;
    QString category;

    //gather the categories
    foreach(gameHack, xml.hacks) {
            if(!categories.contains(gameHack.category)) {
                categories.append(gameHack.category);
            }
    }

    //build the page.


    foreach(category, categories) {

        QWidget * thisPage = new QWidget();
        QScrollArea *m_area = new QScrollArea(thisPage);
        QVBoxLayout *m_layout = new  QVBoxLayout();
        QWidget * contents = new QWidget;
        QVBoxLayout * layout = new QVBoxLayout(contents);

/*
 *  Layout testing...
 *
        thisPage->setStyleSheet("QWidget { background-color: green; }");
        m_area->setStyleSheet("QWidget { background-color: blue; }");
        contents->setStyleSheet("QWidget { background-color: red; }");
*/

        m_layout->addWidget(m_area);

        foreach(gameHack, xml.hacks) {
                if(gameHack.category == category) {

                    QCheckBox * check = new QCheckBox(gameHack.name);
                    check->setObjectName(gameHack.id);
                    check->setChecked(settings.value(check->objectName()).toBool());
                    checkList.append(check);
                    layout->addWidget(check);
                }
        }

        layout->addStretch(1);
        layout->setSizeConstraint(QLayout::SetMinimumSize);
        m_area->setWidget(contents);

        m_area->resize(700, 700);
        thisPage->resize(700, 700);

        ui->RepackButton->setText("Repack " + xml.title);
        ui->unpackButton->setText("Unpack " + xml.title);

        ui->cheatTab->addTab(thisPage, category);
    }

    ui->cheatTab->setElideMode(Qt::ElideNone);
    ui->cheatTab->setTabPosition(QTabWidget::North);
    ui->cheatTab->setUsesScrollButtons(true);

    ui->pathEdit->setText(xml.defaultpath);
    if(QFile(xml.defaultpath + "/" + xml.target).exists()) {
        ui->pathEdit->setStyleSheet(VALIDLINEEDITSTYLE);
    } else {
        ui->pathEdit->setStyleSheet("");
    }

}

void CheatUI::enableUnpack(bool enable)
{
    ui->unpackButton->setEnabled(enable);
}

void CheatUI::enableRepack(bool enable)
{
    ui->RepackButton->setEnabled(enable);
}


void CheatUI::on_unpackButton_clicked()
{

    QDEBUG();
    emit unpack(xml);

}

void CheatUI::on_browseButton_clicked()
{
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);

#ifndef __APPLE__

    QString fileName =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                                        xml.defaultpath,

                                                          QFileDialog::ShowDirsOnly |
                                                                        QFileDialog::DontResolveSymlinks);
#else

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open App"),
                                                    xml.defaultpath,
                                                    tr("Apps (*.app)"));
#endif

    if(!fileName.isEmpty()) {
        ui->pathEdit->setText(fileName);
        settings.setValue(xml.md5 +"_defaultpath", fileName);
        if(QFile(fileName + "/" + xml.target).exists()) {
            ui->pathEdit->setStyleSheet(VALIDLINEEDITSTYLE);
            QMessageBox msgBox;
            ui->cheatTab->setEnabled(false);
            msgBox.setText("Ragatron must be restarted to use new path.");
            msgBox.exec();
        } else {
            ui->pathEdit->setStyleSheet("");
        }
    }


}



//#define SETTINGSCHECK(var) settings.setValue( # var , ui-> var ->isChecked())
//#define GETSETTINGSCHECK(var) ui-> var ->setChecked(settings.value(# var).toBool())


void CheatUI::on_RepackButton_clicked()
{

    QSettings settings(SETTINGSFILE, QSettings::IniFormat);
    QCheckBox * check;
    foreach(check, checkList) {
        settings.setValue(check->objectName(), check->isChecked() );
    }

    QDEBUG();
    settings.sync(); //just in case
    emit repack(xml);

}
