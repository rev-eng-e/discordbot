#include "discordbotui.h"
#include "ui_discordbotui.h"
#include <QPicture>
#include <QPixmap>

discordbotui::discordbotui(QWidget *parent) : QMainWindow(parent),ui(new Ui::discordbotui)
{
    ui->setupUi(this);
    botsetup=make<botsetupui>();
    eventviewer=make<eventviewerui>();
    connect(activitylogger::get(),&activitylogger::log,this,[&](QString text)
    {
        ui->activity->addTopLevelItem(new QTreeWidgetItem(QStringList(text)));
    });
    connect(activitylogger::get(),&activitylogger::setAvatar,this,[&](const QByteArray &avatarImage)
    {
        QPixmap avatar;
        avatar.loadFromData(avatarImage);
        ui->avatar->setPixmap(avatar);
    });
    connect(activitylogger::get(),&activitylogger::setTitle,this,[&](const QString title)
    {
        ui->title->setText(title);
    });
    connect(activitylogger::get(),&activitylogger::addBotInstances,this,[&](const QStringList instanceNames)
    {
        int i=ui->botSelect->currentIndex();
        i=((i >= 0) ? i : 0);
        ui->botSelect->clear();
        ui->botSelect->addItems(instanceNames);
        if(i < ui->botSelect->count())
            ui->botSelect->setCurrentIndex(i);
        else
            ui->botSelect->setCurrentIndex(ui->botSelect->count()-1);

    });

    bot_instances::init();
    activitylogger::get()->addBotInstances(bot_instances::getInstancePaths());

    botcommander::start();
}

discordbotui::~discordbotui()
{
    botcommander::stop();
    if(botsetup)
    {
        botsetup->close();
        botsetup.reset();
    }
    if(eventviewer)
    {
        eventviewer->close();
        eventviewer.reset();
    }
    delete ui;
}

void discordbotui::on_buttonSettings_clicked()
{
    (botsetup->isVisible() ? botsetup->hide() : showBotSetup());
}

void discordbotui::showBotSetup()
{
    botsetup->updateGui();
    botsetup->show();
}

void discordbotui::on_botSelect_currentIndexChanged(int index)
{
    auto bots=bot_instances::get();
    if((size_t)index < bots->bots.size())
    {
        auto &b=bots->bots.at(index);
        activitylogger::get()->setTitle(b->botName);
        activitylogger::get()->setAvatar(b->bot_avatar);
        qDebug() << "Setting bot avatar:" << b->bot_avatar;
        activitylogger::get()->activateMessagePlaybackForBotPath(b->botPaths.botPath);
    }
    bots->release();
}

void discordbotui::on_buttonEvents_clicked()
{
    (eventviewer->isVisible() ? eventviewer->hide() : showEventViewer());
}

void discordbotui::showEventViewer()
{
    eventviewer->show();
}
