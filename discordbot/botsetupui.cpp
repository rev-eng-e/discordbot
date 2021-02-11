#include "botsetupui.h"
#include "ui_botsetupui.h"

botsetupui::botsetupui(QWidget *parent) : QMainWindow(parent),ui(new Ui::botsetupui)
{
    ui->setupUi(this);
    connect(activitylogger::get(),&activitylogger::setBotName,this,[&](const QString botname)
    {
        ui->botNameEdit->setText(botname);
    });
    connect(activitylogger::get(),&activitylogger::setBotKey,this,[&](const QString botkey)
    {
        ui->botKeyEdit->setText(botkey);
    });
}

botsetupui::~botsetupui()
{
    delete ui;
}

void botsetupui::on_botAddButton_clicked()
{
    QString botname=ui->botNameEdit->text();
    QString botkey=ui->botKeyEdit->text();
    addBot(botname,botkey,ui->tts->isChecked());
}

void botsetupui::on_botRemoveButton_clicked()
{
    int selectedSize=ui->bots->selectedItems().size();
    for(int i=0; i < selectedSize; i++)
    {
        QTreeWidgetItem *item=ui->bots->selectedItems().takeAt(i);
        bot_instances::get()->removeBot(item->text(1));
        activitylogger::get()->addBotInstances(bot_instances::getInstancePaths());
        if(item) delete item;
    }
    bot_instances::release();
}

void botsetupui::updateGui()
{
    ui->bots->clear();
    auto bots=bot_instances::get();
    ui->botsFileEdit->setText(bot::getFileName(bots->botsFilePath));
    for(auto &b : bots->bots)
    {
        addBot(b->botName,b->botKey,b->tts);
    }
    bots->release();
}

void botsetupui::addBot(QString botname,QString botkey,bool tts)
{
    if(bot::isNotEmptyOrNull(botname) && bot::isNotEmptyOrNull(botkey))
    {
        for(int i=0; i < ui->bots->topLevelItemCount(); i++)
        {
            QString itemBotKey=ui->bots->topLevelItem(i)->text(1);
            if(botkey==itemBotKey)
            {
                //Update bot configuration
                ui->bots->topLevelItem(i)->setText(0,botname);
                ui->bots->topLevelItem(i)->setText(2,tts ? "yes" : "no");
                saveBot(botname,botkey,tts,i);

            }
        }
        //Add new bot configuration
        QTreeWidgetItem *item=(new QTreeWidgetItem(QStringList() << botname << botkey << (tts ? "yes" : "no")));
        ui->bots->addTopLevelItem(item);
        saveBot(botname,botkey,tts,(ui->bots->topLevelItemCount()-1));
    }
}

void botsetupui::saveBot(QString botname,QString botkey,bool tts,int index)
{
    auto bots=bot_instances::get();
    QString bot_path=bots->getPathForInstance(index);
    bots->addOrUpdateBot(bot_path,botname,botkey,tts);
    bots->addOrUpdateBot(bot_path,botname,botkey,tts);
    bots->release();
    activitylogger::get()->addBotInstances(bot_instances::getInstancePaths());
}

void botsetupui::on_bots_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    ui->botNameEdit->setText(item->text(0));
    ui->botKeyEdit->setText(item->text(1));
    ui->tts->setChecked(item->text(2)=="yes");
}
