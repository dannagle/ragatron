#ifndef CHEATUI_H
#define CHEATUI_H

#include <QWidget>
#include <QTabWidget>
#include <QStringList>
#include <QString>
#include <QHash>
#include <QList>
#include <QCheckBox>
#include "hackstruct.h"
#include "globals.h"

namespace Ui {
class CheatUI;
}

class CheatUI : public QWidget
{
    Q_OBJECT

public:
    explicit CheatUI(QWidget *parent = 0);
    ~CheatUI();
    void generate();
    xml_t xml;

    void enableUnpack(bool enable);
    void enableRepack(bool enable);

signals:
    void unpack(xml_t xml);
    void repack(xml_t xml);

private slots:
    void on_unpackButton_clicked();
    void on_browseButton_clicked();
    void on_RepackButton_clicked();

private:
    Ui::CheatUI *ui;
    QList<QCheckBox *> checkList;


};

#endif // CHEATUI_H
