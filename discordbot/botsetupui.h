#ifndef BOTSETUPUI_H
#define BOTSETUPUI_H

#include <QMainWindow>
#include <QTreeWidget>
#include "discordbot.h"

namespace Ui {
class botsetupui;
}

class botsetupui : public QMainWindow
{
    Q_OBJECT

public:
    explicit botsetupui(QWidget *parent = nullptr);
    ~botsetupui();
    void updateGui();

private slots:
    void on_botAddButton_clicked();
    void on_botRemoveButton_clicked();
    void addBot(QString botname,QString botkey,bool tts);
    void saveBot(QString botname,QString botkey,bool tts,int index);
    void on_bots_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::botsetupui *ui;
};

#endif // BOTSETUPUI_H
