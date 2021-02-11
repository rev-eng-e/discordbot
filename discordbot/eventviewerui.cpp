#include "eventviewerui.h"
#include "ui_eventviewerui.h"

eventviewerui::eventviewerui(QWidget *parent) : QMainWindow(parent),ui(new Ui::eventviewerui)
{
    ui->setupUi(this);
    ui->liveCheckbox->click();
    connect(activitylogger::get(),&activitylogger::activateMessagePlaybackForBotPath,this,&eventviewerui::activateMessagePlayback);
}
eventviewerui::~eventviewerui()
{
    if(timeKeeper)
    {
        timeKeeper->stop();
        timeKeeper.reset();
    }
    if(playbackThread)
    {
        playbackThread->stop();
        playbackThread.reset();
    }
    delete ui;
}

void eventviewerui::activateMessagePlayback(const QString botpath)
{
    activeBotPath=botpath;
    messagesPath=activeBotPath+"/messages";
    setWindowTitle(messagesPath+"/*");
    playbackInfoPath=messagesPath+"/playbackinfo.json";
    if(!ui->liveCheckbox->isChecked())
    {
        ui->usersTree->clear();
        ui->messagesTree->clear();
        startPlayback();
    }
}

void eventviewerui::startPlayback(bool fromBeginning)
{
    if(playbackThread && playbackThread->isRunning()) return;
    if(fromBeginning) loadPlaybackInfo();
    timeKeeper=make<SuspendableThread>([&]
    {
        ui->playbackTime->setDateTime(playbackTime);
        Waiter w(250);
        while(w.timeNotElapsed() && !timeKeeper->shouldStop()) { }
        playbackTime=playbackTime.addMSecs((ui->playbackSpeed->value()*1000)/4);
    });
    playbackThread=make<SuspendableThread>([&]
    {
        qDebug() << "currentMessagesIndex:" << currentMessagesIndex;
        size_t localizedIndex=messagesinfo.indexToLocalizedIndex(currentMessagesIndex);
        qDebug () << "localizedIndex:" << localizedIndex;
        for(size_t i=localizedIndex; i < currentFile.size(); i++)
        {
            json messageWrapped=currentFile.at(i);
            json msg=messageWrapped["m"];
            QString messagetimestamp=j::unquote(msg["d"]["timestamp"]);
            QString mymessagetimestamp=j::unquote(messageWrapped["t"]);
            QDateTime messagetime;
            if(!messagetimestamp.isEmpty()) messagetime=QDateTime::fromString(messagetimestamp,Qt::DateFormat::ISODate);
            if(!mymessagetimestamp.isEmpty()) messagetime=QDateTime::fromString(mymessagetimestamp);

            if(messagetime > playbackTime)
            {
                int secondsUntil=playbackTime.secsTo(messagetime);
                if(secondsUntil > (skipAheadThreshold*60))
                {
                    ui->messagesTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << "" << "Next message is a long time away, skipping ahead in 3 seconds..."));
                    Waiter w(3*1000);
                    while(w.timeNotElapsed()){ if(playbackThread->shouldStop()) break; }
                    playbackTime=messagetime;
                }
                QThread::msleep(10);
                break;
            }
            qDebug() << "messagetime:" << messagetime << "playback time:" << playbackTime
                     << "timestamp:" << messagetimestamp;
            QString author=j::unquote(msg["d"]["author"]["username"]);
            QString content=j::unquote(msg["d"]["content"]);
            QString id=j::unquote(msg["d"]["author"]["id"]);
            QString embedTitle,embed;
            for(auto &e : msg["d"]["embeds"])
            {
                embedTitle=j::unquote(e["title"]);
                embed=j::unquote(e["description"]);
                qDebug() << "embeddedTitle:" << embedTitle << "embed:" << embed;
            }

            if(bot::isNotEmptyOrNull(embedTitle) && bot::isNotEmptyOrNull(embed))
                content+="{"+embedTitle+"}: "+embed;
            QString messageText=("["+messagetime.toString("yyyy-MM-dd hh:mm:ss.zzzAP")+"]: "+content);
            if(bot::isNotEmptyOrNull(id) && bot::isNotEmptyOrNull(author))
            {
                QTreeWidgetItem *item=new QTreeWidgetItem(QStringList() << author << messageText);
                QString avatarpath=(activeBotPath+"/users/avatars/%1.png").arg(id);
                item->setData(0,Qt::DecorationRole,QPixmap(avatarpath));
                ui->messagesTree->addTopLevelItem(item);
            }
            currentMessagesIndex++;
            ui->playbackProgress->setValue(currentMessagesIndex);
        }
        if(currentMessagesIndex >= totalMessages || loadNextFile()==1)
        {
            qDebug() << "playbackTime:" << playbackTime << "currentMessageIndex/totalMessages:" << currentMessagesIndex << totalMessages;
            ui->playbackProgress->setValue(totalMessages);
            ui->messagesTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << "" << "~End of messages~"));
            playbackThread->setShouldStop();
            QThread::msleep(150);
        }
    });
}

void eventviewerui::loadFileForCurrentPlaybackTime()
{
    QDateTime currentPlaybackTime=playbackTime;
    int startx=startRange.date().year();
    int starty=startRange.date().month();
    int startz=startRange.date().day();
    int i=0;
    for(int x=startx; x < (startx+10); x++)
    {
        if(!QDir((messagesPath+"/%1").arg(x)).exists()) continue;
        for(int y=starty; y < 12; y++)
        {
            if(!QDir((messagesPath+"/%1/%2").arg(x).arg(y)).exists()) continue;
            for(int z=startz; z < 32; z++)
            {
                QString nextFilePath=(messagesPath+"/%1/%2/%3.json").arg(x).arg(y).arg(z);
                if(QFile(nextFilePath).exists())
                {
                    if(x==currentPlaybackTime.date().year() && y==currentPlaybackTime.date().month() && z==currentPlaybackTime.date().day())
                    {
                        currentFileIndex=i;
                        currentMessagesIndex=messagesinfo.getMessageIndexForFileIndex(i);
                        qDebug() << "Converted currentplaybackttime to index:" << i << "and messagesindex:" << currentMessagesIndex << "and file day:" << z;
                        qDebug() << "Loading:" << nextFilePath;
                        currentFile=j::fromQString(bot::fileRead(nextFilePath));
                        currentlyLoadedFilePath=nextFilePath;
                        return;
                    }
                    i++;
                }
            }
        }
    }
}

int eventviewerui::loadNextFile()
{
    currentFileIndex=messagesinfo.getFileIndexForMessageIndex(currentMessagesIndex);
    int xx=startRange.date().year();
    int yy=startRange.date().month();
    int zz=startRange.date().day();
    int i=0;
    for(int x=xx; x < (xx+10); x++)
    {
        QDir possibleYear=(messagesPath+"/%1").arg(x);
        if(!possibleYear.exists()) continue;
        for(int y=yy; y < 12; y++)
        {
            QDir possibleMonth=(messagesPath+"/%1/%2").arg(x).arg(y);
            if(!possibleMonth.exists()) continue;
            for(int z=zz; z < 32; z++)
            {
                QString nextFilePath=(messagesPath+"/%1/%2/%3.json").arg(x).arg(y).arg(z);
                if(QFile(nextFilePath).exists())
                {
                    if(i++==currentFileIndex)
                    {
                        if(nextFilePath != currentlyLoadedFilePath)
                        {
                            qDebug() << "Loading:" << nextFilePath;
                            currentFile=j::fromQString(bot::fileRead(nextFilePath));
                            currentlyLoadedFilePath=nextFilePath;
                        }
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

void eventviewerui::loadPlaybackInfo()
{
    QFile f(playbackInfoPath);
    if(f.exists())
    {
       json playbackInfo=j::fromQString(bot::fileRead(playbackInfoPath));
       ui->playbackTime->setDateTime(QDateTime::fromString(j::unquote(playbackInfo["playbacktime"])));
       ui->playbackSpeed->setValue(j::to<int>(playbackInfo["playbackspeed"]));
       ui->playbackProgress->setValue(j::to<int>(playbackInfo["playbackposition"]));
    }
    int startYear=2020,startMonth=1,startDay=1;
    bool startRangeFound=false;
    messagesinfo.clear();
    for(int x=startYear; x < (startYear+10); x++)
    {
        if(!QDir((messagesPath+"/%1").arg(x)).exists()) continue;
        for(int y=startMonth; y < 12; y++)
        {
            if(!QDir((messagesPath+"/%1/%2").arg(x).arg(y)).exists()) continue;
            for(int z=startDay; z < 32; z++)
            {
                QString possibleFile=(messagesPath+"/%1/%2/%3.json").arg(x).arg(y).arg(z);
                if(QFile(possibleFile).exists())
                {
                    json fileJson=j::fromQString(bot::fileRead(possibleFile));
                    size_t startoffset=totalMessages;
                    totalMessages+=fileJson.size();
                    messagesinfo.addNextFileRange(startoffset,totalMessages);
                    if(startRangeFound)
                    {
                        endRange.setDate(QDate(x,y,z));
                        qDebug() << "End Range Set:" << endRange;
                        totalFiles++;
                    }
                    else
                    {
                        startRange.setDate(QDate(x,y,z));
                        endRange.setDate(QDate(x,y,z));
                        startRangeFound=true;
                        totalFiles=1;
                    }
                }
            }
        }
    }
    qDebug() << "Start Range:" << startRange << "End Range:" << endRange << "totalMessagesCount:" << totalMessages << "totalFiles:" << totalFiles;
    QDateTime currentPlaybackTime=ui->playbackTime->dateTime();
    if(currentPlaybackTime <= startRange || currentPlaybackTime >= endRange)
        playbackTime=startRange;
    else
        playbackTime=currentPlaybackTime;
    ui->playbackTime->setDateTime(playbackTime);
    ui->playbackProgress->setMinimum(0);
    ui->playbackProgress->setMaximum(totalMessages);
    loadFileForCurrentPlaybackTime();
    savePlaybackInfo();
}

void eventviewerui::savePlaybackInfo()
{
    json playbackInfo;
    playbackInfo["playbacktime"]=ui->playbackTime->dateTime().toString().toStdString();
    playbackInfo["playbackspeed"]=ui->playbackSpeed->value();
    playbackInfo["playbackposition"]=ui->playbackProgress->sliderPosition();
    bot::fileWrite(j::toQString(playbackInfo).toUtf8(),playbackInfoPath);
}

void eventviewerui::on_liveCheckbox_clicked()
{
    if(ui->liveCheckbox->isChecked())
    {
        ui->playbackTime->setDisabled(true);
        ui->playbackProgress->setDisabled(true);
        ui->playbackSpeed->setDisabled(true);

        savePlaybackInfo();
        if(playbackThread) playbackThread->setShouldStop();
        timeKeeper=make<SuspendableThread>([&]
        {
            ui->playbackTime->setDateTime(QDateTime::currentDateTime());
            Waiter w(1000);
            while(w.timeNotElapsed() && !timeKeeper->shouldStop()) { }
        });
    }
    else
    {
        ui->playbackTime->setDisabled(false);
        ui->playbackProgress->setDisabled(false);
        ui->playbackSpeed->setDisabled(false);
        ui->usersTree->clear();
        ui->messagesTree->clear();
        startPlayback();
    }
}

void eventviewerui::on_playbackProgress_sliderReleased()
{
    currentMessagesIndex=ui->playbackProgress->value();
    loadNextFile();
}
