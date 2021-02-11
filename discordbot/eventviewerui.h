#ifndef EVENTVIEWERUI_H
#define EVENTVIEWERUI_H

#include <QMainWindow>
#include <QLabel>
#include "discordbot.h"

namespace Ui {
class eventviewerui;
}

class messagesrange
{
public:
    size_t startIndex=0,endIndex=0;
    messagesrange(size_t start,size_t end) { startIndex=start; endIndex=end; }
};

class allmessagesinfo
{
public:
    vec<messagesrange> ranges;
    void clear() { ranges.clear(); }
    void addNextFileRange(size_t start,size_t end)
    {
        qDebug() << "Adding range start:" << start << "end:" << end;
        ranges.push_back(messagesrange(start,end));
    }
    size_t getMessageIndexForFileIndex(size_t fileindex)
    {
        size_t i=0;
        for(auto &r : ranges)
        {
            if(i==fileindex)
                return r.startIndex;
            i++;
        }
        return 0;
    }
    size_t getFileIndexForMessageIndex(size_t messageindex)
    {
        size_t i=0;
        for(auto &r : ranges)
        {
            if(messageindex >= r.startIndex && messageindex <= r.endIndex)
                return i;
            i++;
        }
        return messageindex+1;
    }
    size_t indexToLocalizedIndex(size_t index)
    {
        qDebug() << "index:" << index;
        for(auto &r : ranges)
        {
            qDebug() << "r.start=" << r.startIndex << "r.end=" << r.endIndex;
            if(index >= r.startIndex && index <= r.endIndex)
            {
                qDebug() << "r.start=" << r.startIndex << "r.end=" << r.endIndex << "index-r.start=" << index-r.startIndex;
                return index-r.startIndex;
            }
        }
        return index;
    }
};

class eventviewerui : public QMainWindow
{
    Q_OBJECT
public:
    explicit eventviewerui(QWidget *parent=nullptr);
    ~eventviewerui();
public slots:
    void activateMessagePlayback(const QString botpath);
private slots:
    void on_liveCheckbox_clicked();
    void on_playbackProgress_sliderReleased();
private:
    Ui::eventviewerui *ui;
    ptr<SuspendableThread> timeKeeper,playbackThread;
    discordusers users;
    allmessagesinfo messagesinfo;
    QString activeBotPath,messagesPath,playbackInfoPath,currentlyLoadedFilePath;
    QDateTime startRange,endRange,playbackTime;
    json currentFile;
    int currentFileIndex=0,currentMessagesIndex=0,totalFiles=0,totalMessages=0,skipAheadThreshold=10;
    void loadFileForCurrentPlaybackTime();
    int loadNextFile();
    void startPlayback(bool fromBeginning=true);
    void loadPlaybackInfo();
    void savePlaybackInfo();
};

#endif // EVENTVIEWERUI_H
