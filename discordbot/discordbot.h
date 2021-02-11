#ifndef DISCORDBOT_H
#define DISCORDBOT_H

#include <QPointer>
#include <QTimer>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <memory>
#include <vector>
#include "suspendable.h"
#include "json.hpp"
#include "httpsclient.h"

template<class T> using ptr=std::unique_ptr<T>;
template<class T> using sptr=std::shared_ptr<T>;
template<class T> using wptr=std::weak_ptr<T>;
template<class T> using vec=std::vector<T>;
template<class T> using p2v=ptr<vec<T>>;
template<class T> using v2p=vec<ptr<T>>;
template<class T,class ...Args> ptr<T> make(Args... args) { return std::make_unique<T>(args...); }
using json=nlohmann::json;

/*Redesign Blueprint: (Main change is from asynchronous single-threaded into asynchronous multi-threaded)
*---Lightweight simple c++ https communication
    httpsclient [x]
    httpsrequest [x]
    httpsresponse [x]
*---Lightweight simple c++ discord api
    discordbot [x]
    discorduser [x]
    discordmessage [x]
*---Bot commander to create and automatically manage bot instances and their threads
    /w command function vectors and message/command string to mapped function executer
    command [x]
    commandarray [x]
    botcommander [x]
1.0 Release Features Checklist:
    -->Settings and data persistence [x]
    -->Github file fetch and post (in up to 7 messages (4 by default), or it uploads as a text file) [x]
    -->Stack overflow search [x]
    -->Bitcoin price checker [x]
    -->Implement btcgame
2.0 Release Redesign
*---Auto managing websocket service
    WebSocketService [x]
    -->Convert to multi-bot multi-threaded [x]
    -->Multiple auto managed websocket services [x]
*/
template<class T> class Singleton
{
private:
    inline static T *instance;
public:
    Singleton() { }
    ~Singleton() { }
    Singleton(Singleton&)=delete;
    void operator=(Singleton&)=delete;
    inline static void init()
    {
        get();
    }
    inline static T* get()
    {
        if(instance==nullptr)
            instance=new T();
        return instance;
    }
    inline static void free()
    {
        if(instance!=nullptr)
        {
            delete instance;
            instance=nullptr;
        }
    }
};

template<class T> class LockedSingleton
{
private:
    inline static T *instance=nullptr;
    std::atomic<bool> locked=false;
    QMutex mutex;
public:
    LockedSingleton() { }
    ~LockedSingleton() { }
    LockedSingleton(LockedSingleton&)=delete;
    void operator=(LockedSingleton&)=delete;
    inline static void init()
    {
        get();
        release();
    }
    inline static T* get()
    {
        if(instance==nullptr)
            instance=new T();
        if(!instance->locked)
        {
            instance->mutex.lock();
            instance->locked=true;
        }
        return instance;
    }
    inline static T* release()
    {
        if(instance!=nullptr && instance->locked)
        {
            instance->mutex.unlock();
            instance->locked=false;
        }
        return nullptr;
    }
    inline static void free()
    {
        if(instance!=nullptr)
        {
            if(instance->locked) release();
            delete instance;
            instance=nullptr;
        }
    }
};

class activitylogger : public QObject
{
    Q_OBJECT
signals:
    void log(const QString text);
    void setAvatar(const QByteArray &avatarImage);
    void setTitle(const QString title);
    void setBotName(const QString botname);
    void setBotKey(const QString botkey);
    void activateMessagePlaybackForBotPath(const QString botpath);
    void addBotInstances(const QStringList instanceNames);
private:
    inline static ptr<activitylogger> activity=nullptr;
public:
    activitylogger() { }
    ~activitylogger() { }
    activitylogger(activitylogger&)=delete;
    void operator=(activitylogger&)=delete;
    inline static activitylogger* get()
    {
        if(activity==nullptr)
            activity=make<activitylogger>();
        return activity.get();
    }
    inline static void dbg(const QString &text)
    {
        qDebug() << text;
        get()->log(text);
    }
};

class j
{
public:
    inline static QString escape(QString string)
    {
        if(string.isEmpty()) return string;
        string.replace("\r","\\r");
        string.replace("\n","\\n");
        string.replace("\t","\\t");
        string.replace("\b","\\b");
        string.replace("\f","\\f");
        string.replace("\"","\\\"");
        return string;
    }
    inline static QString unescape(QString string)
    {
        if(string.isEmpty()) return string;
        string.replace("\\r","\r");
        string.replace("\\n","\n");
        string.replace("\\t","\t");
        string.replace("\\b","\b");
        string.replace("\\f","\f");
        string.replace("\\\"","\"");
        return string;
    }
    inline static QString unquote(json &j)
    {
        QString js=toQString(j);
        js.replace("\"","");
        return js;
    }
    template<class T> inline static T to(json &j)
    {
        T value;
        try
        {
            value=j;
        }
        catch (json::type_error &e)
        {
            qDebug() << "json::type_error while reading a value:" << e.what() << "for:" << toQString(j);
        }
        return value;
    }
    inline static QString toQString(json &j)
    {
        QString js;
        try
        {
            js=j.dump().c_str();
        }
        catch (json::type_error &e)
        {
            qDebug() << "json::type_error while dumping to c_str:" << e.what();
        }
        return js;
    }
    inline static json fromQString(QString js)
    {
        json j;
        try
        {
            j=json::parse(js.toStdString());
        }
        catch (json::parse_error &e)
        {
            qDebug() << "json::parse_error:" << e.what();
        }
        return j;
    }
};

class botpaths
{
public:
    QString botPath,messagesPath,usersPath,avatarsPath,avatarPath,configPath;
    botpaths() { }
    botpaths(QString bot_path)
    {
        botPath=bot_path;
        messagesPath=bot_path+"/messages";
        usersPath=bot_path+"/users";
        avatarsPath=bot_path+"/users/avatars";
        avatarPath=bot_path+"/bot_avatar.png";
        configPath=bot_path+"/bot_config.json";
    }
};

class bot
{
public:
    botpaths botPaths;
    QString botName,botKey;
    QByteArray bot_avatar,bot_config;
    bool tts=false;
    void setPaths(QString bot_path)
    {
        botPaths=botpaths(bot_path);
        bot::makeIfNotExists(botPaths.messagesPath);
        bot::makeIfNotExists(botPaths.avatarsPath);
    }
    void load()
    {
        if(isNotEmptyOrNull(botPaths.configPath)) loadConfig(botPaths.configPath);
    }
    void save()
    {
        if(isNotEmptyOrNull(botPaths.configPath)) saveConfig(botPaths.configPath);
    }
    void loadConfig(QString config_path)
    {
        QFile f(config_path);
        if(!f.exists()) saveConfig(config_path);
        bot_config=fileRead(config_path);
        qDebug() << "Loaded botconfig! Size:" << bot_config.size();
        if(bot_config.size() > 0)
        {
            json cfg=j::fromQString(bot_config);
            botName=j::unquote(cfg["name"]);
            botKey=j::unquote(cfg["key"]);
            tts=(j::unquote(cfg["tts"])=="yes");
            qDebug() << "Loaded:" << botName << botKey << tts;
        }
        if(isNotEmptyOrNull(botPaths.avatarPath)) bot_avatar=fileRead(botPaths.avatarPath);
    }
    void saveConfig(QString config_path)
    {
        QString botcfg=QString(R"({"name":"%1","key":"%2","tts":"%3"})").arg(botName).arg(botKey).arg(tts ? "yes" : "no");
        json cfg=j::fromQString(botcfg);
        QString bot_config=j::toQString(cfg);
        qDebug() << "bot_config:" << bot_config;
        fileWrite(bot_config.toUtf8(),config_path);
        if(bot_avatar.size() > 0 && isNotEmptyOrNull(botPaths.avatarPath)) fileWrite(bot_avatar,botPaths.avatarPath);
    }
    inline static bool isEmptyOrNull(QString str) { return (str=="" || str=="null"); }
    inline static bool isNotEmptyOrNull(QString str) { return (str != "" && str != "null"); }
    inline static bool makeIfNotExists(QString path)
    {
        QDir dir=QDir(path);
        if(!dir.exists()) return dir.mkpath(path);
        return false;
    }
    inline static QByteArray fileRead(QString path,bool *successful=nullptr)
    {
        QFile f(path);
        if(f.size() > 0 && f.open(QIODevice::ReadOnly))
        {
            QByteArray file=f.readAll();
            if(file.size()==f.size())
            {
                if(successful != nullptr)
                    *successful=true;
                f.close();
                return file;
            }
            f.close();
        }
        if(successful != nullptr)
            *successful=false;
        return QByteArray();
    }
    inline static bool fileWrite(QByteArray file,QString path,bool append=false)
    {
        QFile f(path);
        if(f.open(QIODevice::WriteOnly))
        {
            if(append) f.seek(f.size());
            qint64 actualBytesWritten=f.write(file);
            if(actualBytesWritten==0 || actualBytesWritten==-1)
            {
                f.close();
                return false;
            }
            qint64 writeOffset=actualBytesWritten,bytesRemaining=(file.size()-actualBytesWritten);
            while(bytesRemaining > 0)
            {
                actualBytesWritten=f.write(file.data()+writeOffset,bytesRemaining);
                if(actualBytesWritten==0 || actualBytesWritten==-1)
                    break;
                bytesRemaining-=actualBytesWritten;
                writeOffset+=actualBytesWritten;
            }
            if(f.size()==file.size())
            {
                f.close();
                return true;
            }
            f.close();
        }
        return false;
    }
    inline static QString getFileName(QString filepath)
    {
        int lastSlash=filepath.lastIndexOf("/");
        if(lastSlash != -1)
            return filepath.right(filepath.size()-(lastSlash+1));
        return "";
    }

    inline static QString getFileExtension(QString filename)
    {
        int lastDot=filename.lastIndexOf(".");
        if(lastDot != -1)
            return filename.right(filename.size()-lastDot);
        return "";
    }
    inline static QString prettifyMoneyString(QString moneyString)
    {
        int startAt=moneyString.size();
        int decimalpoint=moneyString.indexOf(".");
        if(decimalpoint != -1)
            startAt=decimalpoint;
        for(int x=0,i=startAt; i > 0; i--,x++)
        {
            if(x==3)
            {
                moneyString.insert(i,",");
                x=0;
            }
        }
        return moneyString;
    };
};

class bot_instances : public LockedSingleton<bot_instances>
{
public:
    inline static QString zeroPath,botFilePath,botsFilePath,defaultBotPath,avatarsUrl="https://cdn.discordapp.com/avatars/%1/%2.png?size=128";
    inline static qint64 concurrency=std::thread::hardware_concurrency();
    inline static qint64 thread_allowance;
    v2p<bot> bots;
    QByteArray bot_file,bots_file;

    bot_instances() { load(); }
    ~bot_instances() { save(); }
    bot_instances(bot_instances&)=delete;
    void operator=(bot_instances&)=delete;
    inline static QString getPathForInstance(int i=0) { return (zeroPath+"/instance_%1").arg(i); }
    inline static QStringList getInstancePaths()
    {
        auto instances=get();
        QStringList paths;
        for(size_t i=0; i < instances->bots.size(); i++)
        {
            paths+=getPathForInstance(i);
        }
        instances->release();
        return paths;
    }
    void save()
    {
        for(auto &b : bots)
            b->save();
        saveBotsFile();
        saveBotFile();
    }
    void load()
    {
        zeroPath=QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
        botFilePath=zeroPath+"/bot.json";
        botsFilePath=zeroPath+"/bots.json";
        defaultBotPath=getPathForInstance(0);
        loadBotFile();
        loadBotsFile();
        for(auto &b : bots)
            b->load();
        refreshThreadAllowance();
    }
    void refreshThreadAllowance()
    {
        qint64 numBots=bots.size();
        thread_allowance=concurrency/numBots;
        if(thread_allowance < 0)
            thread_allowance=1;
        qDebug() << "Thread allowance:" << thread_allowance;
    }
    void loadBotFile()
    {
        QFile f(botFilePath);
        if(!f.exists())
        {
            bot_file=QString(R"({"bots_file":"%1"})").arg(botsFilePath).toUtf8();
            saveBotFile();
        }
        else
        {
            bot_file=bot::fileRead(botFilePath);
        }
        json bot_file_json=j::fromQString(bot_file);
        QString botsfile=j::unquote(bot_file_json["bots_file"]);
        if(bot::isNotEmptyOrNull(botsfile))
        {
            botsFilePath=botsfile;
            qDebug() << "Loaded bot file! bots file path:" << botsFilePath;
        }
    }
    void saveBotFile()
    {
        if(bot_file.size() > 0 && !bot::isEmptyOrNull(botFilePath))
            bot::fileWrite(bot_file,botFilePath);
    }
    void addOrUpdateBot(QString bot_path,QString name,QString key,bool tts)
    {
        qDebug() << "Add or update bot..." << bot_path << name << key << tts;
        if(bot::isNotEmptyOrNull(bot_path) && bot::isNotEmptyOrNull(name) && bot::isNotEmptyOrNull(key))
        {
            for(auto &b : bots)
            {
                if(b->botPaths.botPath==bot_path)
                {
                    qDebug() << "Updating...";
                    b->setPaths(bot_path);
                    b->botName=name;
                    b->botKey=key;
                    b->tts=tts;
                    save();
                    return;
                }
            }
            qDebug() << "Adding...";
            ptr<bot> instance=make<bot>();
            instance->botName=name;
            instance->botKey=key;
            instance->tts=tts;
            instance->setPaths(bot_path);
            bots.push_back(std::move(instance));
            save();
        }
    }
    void removeBot(QString key)
    {
        int i=0;
        for(auto &b : bots)
        {
            if(b->botKey==key)
            {
                bots.erase(bots.begin()+i);
                break;
            }
            i++;
        }
    }
    void loadBotsFile()
    {
        QFile f(botsFilePath);
        if(!f.exists())
        {
            bots_file=QString(R"([{"bot_path":"%1"}])").arg(defaultBotPath).toUtf8();
            bot::fileWrite(bots_file,botsFilePath);
        }
        else
        {
            bots_file=bot::fileRead(botsFilePath);
        }
        json bots_json=j::fromQString(bots_file);
        bots.clear();
        for(auto &b : bots_json)
        {
            QString bot_path=j::unquote(b["bot_path"]);
            qDebug() << "botPath:" << bot_path;
            if(bot::isNotEmptyOrNull(bot_path))
            {
                ptr<bot> instance=make<bot>();
                instance->setPaths(bot_path);
                qDebug() << "Loaded bot at:" << instance->botPaths.botPath;
                bots.push_back(std::move(instance));
            }
        }
    }
    void saveBotsFile()
    {
        bots_file.clear();
        bots_file.append("[");
        for(auto &b : bots)
        {
            QString bot_path=QString(R"({"bot_path":"%1"},)").arg(b->botPaths.botPath);
            bots_file.append(bot_path);
        }
        bots_file.truncate(bots_file.size()-1);
        bots_file.append("]");
        if(bot::isNotEmptyOrNull(botsFilePath))
            bot::fileWrite(bots_file,botsFilePath);
    }
    bot* botByKey(QString key)
    {
        for(auto &b : bots)
        {
            if(b->botKey==key)
                return b.get();
        }
        return nullptr;
    }
};

class discorduser
{
public:
    QString id,name,discriminator,avatar,last_channel_id,last_guild_id,status="online";
    QByteArray avatarImage,avatarHash,user_hash;
    bool isBot=false,isStreaming=false,onCamera=false;

    discorduser() { }
    void calculateHash()
    {
        user_hash=QCryptographicHash::hash(id.toUtf8(),QCryptographicHash::Sha256);
    }

    void loadAvatarImageFromFile(QString avatarsPath)
    {
        qDebug() << "avatarsPath:" << avatarsPath;
        QString avatarPath=(avatarsPath+"/%1.png").arg(id);
        bool successful=false;
        avatarImage=bot::fileRead(avatarPath,&successful);
        if(successful)
            avatarHash=QCryptographicHash::hash(avatarImage,QCryptographicHash::Sha256);
    }
    void downloadAvatarImage(QString avatarsPath)
    {
        if(bot::isEmptyOrNull(id) || bot::isEmptyOrNull(avatar)) return;
        qDebug() << "avatarsPath:" << avatarsPath;
        loadAvatarImageFromFile(avatarsPath);
        if(avatarImage.size()==0 || QRandomGenerator().bounded(1,10)==10)
        {
            httpsclient https;
            QString avatar_url=bot_instances::avatarsUrl.arg(id).arg(avatar);
            auto response=https.send(httpsrequest(httpsrequest::type::GET,avatar_url));
            if(response.success && !response.content.isEmpty())
            {
                QByteArray newAvatarImage=response.content;
                QByteArray newAvatarHash=QCryptographicHash::hash(newAvatarImage,QCryptographicHash::Sha256);
                if(newAvatarHash != avatarHash)
                {
                    QString avatarPath=(avatarsPath+"/%1.png").arg(id);
                    bot::fileWrite(newAvatarImage,avatarPath);
                    avatarImage=newAvatarImage;
                    avatarHash=newAvatarHash;
                    qDebug() << "Wrote new avatar image to disk for user:" << name << "size:" << avatarImage.size() << "path:" << avatarPath << "url:" << avatar_url;
                }
            }
        }
    }
};

class discordusers
{
public:
    vec<discorduser> users;
    botpaths botPaths;
    QMutex mutex;
    discordusers() { }
    ~discordusers() { }
    bool updateStatusForUser(json &j)
    {
        QMutexLocker locker(&mutex);
        QString userid=j::unquote(j["user"]["id"]);
        QString status=j::unquote(j["status"]);
        for(auto &usr : users)
        {
            if(usr.id==userid)
            {
                usr.status=status;
                qDebug() << "Present user:" << userid << "status:" << status;
                return true;
            }
        }
        return false;
    }
    discorduser updateStreamingStatusForUser(json &j)
    {
        QMutexLocker locker(&mutex);
        QString userid=j::unquote(j["member"]["user"]["id"]);
        QString streamingStatus=j::unquote(j["self_stream"]);
        QString onCameraStatus=j::unquote(j["self_video"]);
        qDebug() << "1Voice state update user:" << userid << "streaming:" << streamingStatus << "on camera:" << onCameraStatus;
        for(auto &usr : users)
        {
            if(usr.id==userid)
            {
                usr.isStreaming=streamingStatus=="true" ? true : false;
                usr.onCamera=onCameraStatus=="true" ? true : false;
                qDebug() << "2Voice state update user:" << userid << "streaming:" << streamingStatus << "on camera:" << onCameraStatus;
                discorduser u=usr;
                return u;
            }
        }
        return discorduser();
    }
    discorduser addUserFromGuildCreate(json &j)
    {
        QMutexLocker locker(&mutex);
        discorduser user;
        QString avatar=j::unquote(j["user"]["avatar"]);
        user.id=j::unquote(j["user"]["id"]);
        for(auto &usr : users)
        {
            if(usr.id==user.id)
            {
                //Update user's avatar in cased changed
                usr.avatar=avatar;
                usr.downloadAvatarImage(botPaths.avatarsPath);
                user=usr;
                qDebug() << "[GUILD_CREATE] updated member:" << user.name << user.discriminator << "id: "<< user.id << "avatar:" << avatar << "avatar image size:" << user.avatarImage.size();
                return user;
            }
        }
        //Haven't seen this user before, add them from this guild create message
        user.name=j::unquote(j["user"]["username"]);
        user.discriminator=j::unquote(j["user"]["discriminator"]);
        user.avatar=j::unquote(j["user"]["avatar"]);
        user.downloadAvatarImage(botPaths.avatarsPath);
        bool isBot=false;
        if(j["user"]["bot"] != nullptr)
            isBot=j::to<bool>(j["user"]["bot"]);
        users.push_back(user);
        qDebug() << "[GUILD_CREATE] member:" << user.name << user.discriminator << "id:" << user.id << "avatar:" << avatar << "bot:" << isBot;
        return user;
    }
    discorduser addUserFromMessage(json &j)
    {
        QMutexLocker locker(&mutex);
        discorduser user;
        QString channel=j::unquote(j["d"]["channel_id"]);
        QString avatar=j::unquote(j["d"]["author"]["avatar"]);
        user.id=j::unquote(j["d"]["author"]["id"]);
        for(auto &usr : users)
        {
            if(usr.id==user.id)
            {
                //Update user's last seen in channel and avatar if either changed
                if(usr.last_channel_id != channel)
                    usr.last_channel_id=channel;
                if(usr.avatar != avatar)
                {
                    usr.avatar=avatar;
                    usr.downloadAvatarImage(botPaths.avatarsPath);
                }
                user=usr;
                qDebug() << "[MESSAGE_CREATE] Updated user from message create:" << usr.name << usr.discriminator << "id:" << usr.id << "avatar:" << usr.avatar << "avatar image size:" << usr.avatarImage.size();
                return user;
            }
        }
        //Haven't seen this user before, add them from this discord message
        user.name=j::unquote(j["d"]["author"]["username"]);
        user.discriminator=j::unquote(j["d"]["author"]["discriminator"]);
        user.avatar=avatar;
        user.last_channel_id=channel;
        user.downloadAvatarImage(botPaths.avatarsPath);
        user.calculateHash();
        users.push_back(user);
        qDebug() << "[MESSAGE_CREATE] Added user from message create:" << user.name << user.discriminator << "id:" << user.id << "avatar:" << user.avatar << "avatar image size:" << user.avatarImage.size();
        return user;
    }
};

class discordmessage
{
public:
    json jsonmsg;
    QString stringmsg,cmd,usercmd,channel_id,usermsg;
    QByteArray cmd_id_hash,usercmd_id_hash,msg_hash;
    QDateTime createdAt;
    uint32_t id=0,completed=0;
    bool handledDiscordCommand=false,handledUserCommand=false,hasUserCommand=false;
    discordmessage(QString message)
    {
        if(message.isEmpty())
            return;
        createdAt=QDateTime::currentDateTime();
        jsonmsg=j::fromQString(message);
        stringmsg=message;
        id=j::to<uint32_t>(jsonmsg["op"]);
        cmd=j::unquote(jsonmsg["t"]);
        if(jsonmsg["d"]!=nullptr)
        {
            auto d=jsonmsg["d"];
            usermsg=j::unquote(d["content"]);
            channel_id=j::unquote(d["channel_id"]);
        }
        if(usermsg.contains(" "))
            usercmd=usermsg.split(" ",Qt::SkipEmptyParts)[0].toLower();
        else
            usercmd=usermsg.toLower();
        hasUserCommand=(id==0 && cmd=="MESSAGE_CREATE" && usercmd != "null");
        calculateHashes();
    }
    void calculateHashes()
    {
        QString hashCommandId=QString("%1+%2").arg(cmd).arg(id);
        QString hashUserCommandId=QString("%1+%2").arg(usercmd).arg(id);
        msg_hash=QCryptographicHash::hash(stringmsg.toUtf8(),QCryptographicHash::Sha256);
        cmd_id_hash=QCryptographicHash::hash(hashCommandId.toUtf8(),QCryptographicHash::Sha256);
        if(hasUserCommand)
            usercmd_id_hash=QCryptographicHash::hash(hashUserCommandId.toUtf8(),QCryptographicHash::Sha256);
        //qDebug() << "message op:" << id << "cmd:" << cmd << "usercmd:" << usercmd << "cmd_id_hash:" << cmd_id_hash.toHex() << "usercmd_id_hash:" << usercmd_id_hash.toHex();
    }
};

class command
{
public:
    QString cmd;
    quint32 op;
    QByteArray cmd_id_hash;
    std::function<void(discordmessage&)> func;
    bool isUserCommand=false;
    command() { }
    command(QString cmd,quint32 op,std::function<void(discordmessage&)> func,bool isUserCommand=false)
    {
        this->cmd=cmd.toLower();
        this->op=op;
        this->func=func;
        this->isUserCommand=isUserCommand;
        QString hashCommandId=QString("%1+%2").arg(cmd).arg(op);
        cmd_id_hash = QCryptographicHash::hash(hashCommandId.toUtf8(),QCryptographicHash::Sha256);
        qDebug() << "Generated hash:" << cmd_id_hash.toHex() << "from command:" << cmd << "op:" << op;
    }
    bool operator==(discordmessage &msg)
    {            
        if(!isUserCommand && msg.cmd_id_hash==cmd_id_hash && !msg.handledDiscordCommand)
            return msg.handledDiscordCommand=++msg.completed;
        else if(isUserCommand && msg.usercmd_id_hash==cmd_id_hash && !msg.handledUserCommand)
            return msg.handledUserCommand=++msg.completed;
        return false;
    }
};

class commandarray
{
public:
    vec<command> cmds;
};

class commands : public Singleton<commands>
{
public:
    vec<commandarray> cmds;
    std::atomic<bool> commandsadded=false;

    commands()
    {
        commandarray commands;
        cmds.push_back(commands); //Discord commands array
        cmds.push_back(commands); //Custom commands array
    }
    ~commands() { }
    commands(commands&)=delete;
    void operator=(commands&)=delete;
    inline static void onDiscord(QString cmd,uint32_t id,std::function<void(discordmessage&)> func)
    {
        auto commands=get();
        commandarray *cmdarray=&commands->cmds.at(0);
        if(cmdarray!=nullptr)
            cmdarray->cmds.push_back(command(cmd,id,func));
    }
    inline static void onCustom(QString cmd,uint32_t id,std::function<void(discordmessage&)> func)
    {
        auto commands=get();
        commandarray *cmdarray=&commands->cmds.at(1);
        if(cmdarray!=nullptr)
            cmdarray->cmds.push_back(command(cmd,id,func,true));
    }
    inline static void setCommandsAdded() { get()->commandsadded=true; }
    inline static bool commandsAdded() { return get()->commandsadded; }
};

class messagestorage : public QObject
{
    Q_OBJECT
public:
    inline static size_t writeMessagesThreshold=10;
    inline static qint64 nextLogMessagesDelay=60*1000; //Checks whether to write and does write every 60 seconds and if 10 or more completed messages exist...
    vec<discordmessage> messagequeue,messagescompleted;
    QString botDirectory,outputDirectory,currentOutputPath;
    bool append=false;
    int lastWrittenDay=0;
    messagestorage() { }
    ~messagestorage() { }
    void determineOutputDirectoryAndFile()
    {
        QDateTime current=QDateTime::currentDateTime();
        int year=current.date().year();
        int month=current.date().month();
        int day=current.date().day();
        outputDirectory=((botDirectory+"/messages/%1/%2").arg(year).arg(month));
        currentOutputPath=(outputDirectory+"/%1.json").arg(day);
        bot::makeIfNotExists(outputDirectory);
    }
    int getCurrentDay()
    {
        return QDateTime::currentDateTime().date().day();
    }
    bool writeMessagesToDisk()
    {
        if(messagescompleted.empty()) return false;
        if(getCurrentDay() != lastWrittenDay)
        {
            determineOutputDirectoryAndFile();
            lastWrittenDay=getCurrentDay();
        }

        json jsonFile=j::fromQString(bot::fileRead(currentOutputPath));
        for(auto &message : messagescompleted)
        {
            QString wrapped=QString(R"({"t":"%1","m":%2})").arg(message.createdAt.toString()).arg(j::toQString(message.jsonmsg));
            qDebug() << "Wrapped:" << wrapped;
            jsonFile.push_back(j::fromQString(wrapped));
        }
        return bot::fileWrite(j::toQString(jsonFile).toUtf8(),currentOutputPath);
    }
};

class WebSocketService : public Thread<WebSocketWorker>
{
    Q_OBJECT
signals:
    void openConnection(QString url);
    qint64 sendTextMessage(const QString &message);
    void closeConnection();
    void textMessageReceived(QString message);
private:
    ptr<QTimer> autoConnect,autoHeartbeat,autoLogMessages;
    QString websocketurl,botkey,session_id,botDirectory;
    QMutex mutex;
    std::atomic<quint64> checkConnectionDelay=3*1000,timeoutUntil=0,heartbeat_interval=41250,sequence=0,invalid_session_count=0;
public:
    ptr<messagestorage> messages;
    std::atomic<bool> resumable=false,heartbeat_ack=false,messagesWritten=false,probablyBadKey=false;
    explicit WebSocketService(QObject *parent=nullptr,QString botKey="") : Thread<WebSocketWorker>(new WebSocketWorker,parent),botkey(botKey)
    {
        //Signals to slots
        connect(this,&WebSocketService::started,this,&WebSocketService::startTimers);
        connect(this,&WebSocketService::openConnection,worker,&WebSocketWorker::onOpenConnection);
        connect(this,&WebSocketService::sendTextMessage,worker,&WebSocketWorker::onSendTextMessage);
        connect(this,&WebSocketService::closeConnection,worker,&WebSocketWorker::onCloseConnection);
        connect(worker,&WebSocketWorker::textMessageReceived,this,&WebSocketService::onTextMessageReceived);
        //Signal forwarding
        connect(worker,&WebSocketWorker::textMessageReceived,this,&WebSocketService::textMessageReceived);

        messages=make<messagestorage>();
        autoConnect=make<QTimer>();
        autoHeartbeat=make<QTimer>();
        autoLogMessages=make<QTimer>();
        connect(autoConnect.get(),&QTimer::timeout,worker,[&]
        {
            quint64 current_time=QDateTime::currentMSecsSinceEpoch();
            if(worker->connectionattempts >= 3)
            {
                int timeout_time=QRandomGenerator().bounded(60,240) * (worker->connectionfailures+1);
                timeoutUntil=current_time+timeout_time*1000;
                qDebug() << "Connecting failed after" << ((worker->connectionfailures+1)*3) << "attempts, timing out for" << timeout_time << "seconds... on:" << this;
            }
            if(!worker->connected && bot::isNotEmptyOrNull(websocketurl) && !probablyBadKey)
            {
                if(timeoutUntil > current_time)
                    qDebug() << "Still in timeout for" << ((timeoutUntil-current_time) / 1000) << "seconds... on:" << this;
                else
                    openConnection(websocketurl);
            }
        });
        connect(autoHeartbeat.get(),&QTimer::timeout,worker,[&]
        {
            if(worker->connected)
            {
                QString heartbeat=QString(R"({"op":1,"d":%1})").arg(sequence);
                //Send heartbeat
                qDebug() << "Sending heartbeat:" << heartbeat << "from:" << this;
                heartbeat_ack=false;
                sendTextMessage(heartbeat);
            }
        });
        connect(autoLogMessages.get(),&QTimer::timeout,this,&WebSocketService::writeCompletedMessagesToDisk);
        startService();
    }
    ~WebSocketService()
    {
        stopService();
        qDebug() << this << "destructed!";
    }
    void setBotPath(QString path) { messages->botDirectory=botDirectory=path; }
    void setUrl(QString url) { websocketurl=url; }
    void suspendService() { if(!worker->suspended) worker->suspend(); }
    void resumeService() { if(worker->suspended) worker->resume(); }
    void startTimers()
    {
        autoConnect->start(checkConnectionDelay);
        autoLogMessages->start(messagestorage::nextLogMessagesDelay);
    }
    void startService(QString url="")
    {
        if(bot::isNotEmptyOrNull(url)) setUrl(url);
        openConnection(websocketurl);
    }
    void stopService()
    {
        autoConnect->stop();
        autoHeartbeat->stop();
        autoLogMessages->stop();
        closeConnection();
    }
    void onTextMessageReceived(QString textMessage)
    {
        json msg=j::fromQString(textMessage);
        if(msg["s"] != nullptr)
        {
            sequence=j::to<quint32>(msg["s"]);
            qDebug() << "Last received sequence:" << sequence;
        }
        quint32 op=j::to<quint32>(msg["op"]);
        QString t=j::unquote(msg["t"]);
        if(op==9) //Invalid session
        {
            probablyBadKey=(++invalid_session_count > 1 && bot::isEmptyOrNull(session_id));
            resumable=j::to<bool>(msg["d"]);
            if(probablyBadKey)
            {
                stopService();
                invalid_session_count=0;
                probablyBadKey=false;
                activitylogger::dbg(QString("For %1, check/update key! Received invalid session twice without ever successfully authorizing!").arg(bot::getFileName(botDirectory)));
            }
            else
            {
                closeConnection();
                qDebug() << "Received invalid session message, restarting connection... resumable:" << resumable;
            }
        }
        else if(op==10) //Hello
        {
            heartbeat_interval=j::to<quint32>(msg["d"]["heartbeat_interval"]);
            qDebug() << "Received heartbeat interval:" << heartbeat_interval << "for:" << this;
            autoHeartbeat->start(heartbeat_interval-1000);
            if(resumable)
            {
                QString resume=QString(R"({"op":6,"d":{"token":"%1","session_id":"%2","seq":%3}})").arg(botkey).arg(session_id).arg(sequence);
                //Send resume!
                sendTextMessage(resume);
                qDebug() << "Sent resume! : " << j::unescape(resume);
            }
            else
            {
                QString hello=QString(R"({"op":2,"d":{"token":"%1","properties":{"$os":"linux","$browser":"firefox","$device":"laptop"}}})").arg(botkey);
                resumable=true;
                //Send identify!
                sendTextMessage(hello);
                qDebug() << "Sent identify! : " << j::unescape(hello);
            }
        }
        else if(op==11) //Heartbeart acknowledged
        {
            heartbeat_ack=true;
            qDebug() << "Heartbeat acknowledged! :" << j::unescape(textMessage);
        }
        else if(op==0 && t=="RESUMED")
        {
            qDebug() << "Resumed connection! :" << j::unescape(textMessage);
        }
        else if(op==0 && t=="READY")
        {
            session_id=j::unquote(msg["d"]["session_id"]);
        }
        acceptmessage(discordmessage(textMessage));
    }
    void acceptmessage(discordmessage msg)
    {
        for(auto &m : messages->messagequeue)
        {
            if(m.msg_hash==msg.msg_hash) //Dont insert the exact same message more than once
                return;
        }
        messages->messagequeue.push_back(msg);
    }
    void writeCompletedMessagesToDisk()
    {
        messagesWritten=false;
        auto writeMessagesThread=QThread::create([&]
        {
            QMutexLocker locker(&mutex);
            if((messages->messagescompleted.size() > messagestorage::writeMessagesThreshold || SuspendableThread::shouldAllStop())
                    && messages->writeMessagesToDisk())
            {
                messages->messagescompleted.clear();
                messagesWritten=true;
            }
        });
        if(writeMessagesThread)
        {
            connect(writeMessagesThread,&QThread::finished,writeMessagesThread,&QThread::deleteLater);
            writeMessagesThread->start();
        }
    }
};

class stackresult
{
public:
    stackresult() { }
    stackresult(QString Title,QString Link) : title(Title),link(Link) { }
    QString title,link;
};

class stackquery
{
public:
    quint64 resultIndex=0,resultsPerMessage=5,messagesPerResponse=1;
    QString channelId,txtquery;
    vec<stackresult> results;
    vec<QString> resultsMessages;

    stackquery() { }
    stackquery(QString channelId,QString txtquery)
    {
        this->channelId=channelId;
        this->txtquery=txtquery;
    }
    void search()
    {
        httpsclient https;
        https.setUserAgent("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:85.0) Gecko/20100101 Firefox/85.0");
        auto searchquery=QString("https://stackoverflow.com/search?q=%1&s=fb5494b2-c15b-473a-968c-b1d7b57c6d7c").arg(cookiecontainer::urlEncode(txtquery));
        auto request=httpsrequest(httpsrequest::type::GET,searchquery);
        static QString stackoverflowreferer;
        if(stackoverflowreferer.isEmpty())
            request.referer="https://stackoverflow.com/";
        else
            request.referer=stackoverflowreferer;
        request.cookies="prov=147a1733-ae53-f484-3519-72b9c873a840";

        auto response=https.send(request);
        stackoverflowreferer=searchquery;
        if(response.success)
        {
            QRegularExpression rx("<a href=\"(/questions/[0-9]+\\S+)\".*title=\"(\\S+.*)\" class=");
            QRegularExpressionMatchIterator i=rx.globalMatch(response.content);
            while(i.hasNext())
            {
                QRegularExpressionMatch match = i.next();
                QString url=match.captured(1);
                QString title=match.captured(2);
                stackresult result(title,url);
                results.push_back(result);
            }
            resultIndex=0;
            refreshResults();
        }
    }
    void refreshResults()
    {
        resultsMessages.clear();
        QString selectiveResults;
        for(size_t i=0,x=0,z=0; i < results.size(); i++)
        {
            selectiveResults+=resultByIndex(i);
            if(++x==resultsPerMessage)
            {
                resultsMessages.push_back(j::escape(selectiveResults));
                selectiveResults.clear();
                x=0;
                if(++z==messagesPerResponse)
                    break;
            }
        }
        if(selectiveResults.length() > 0)
            resultsMessages.push_back(j::escape(selectiveResults));
    }
    QString resultByIndex(uint64_t index)
    {
        QString resultNumber=QString().asprintf("%llu) ",index+1);
        QString urlString("\nhttps://stackoverflow.com"+results.at(index).link+"\n");
        return resultNumber+decodeXML(results.at(index).title)+urlString;
    }
    QString currentResultMessage()
    {
        if(resultIndex >= resultsMessages.size()) resultIndex=0;
        return resultsMessages.at(resultIndex);
    }
    inline static QString decodeXML(QString decodeMe)
    {
        decodeMe.replace("&#x2B;","+");
        decodeMe.replace("&#13;","");
        decodeMe.replace("&amp;", "&");
        decodeMe.replace("&apos;", "'");
        decodeMe.replace("&#x27;", "'");
        decodeMe.replace("&#39;","'");
        decodeMe.replace("&quot;", "\"");
        decodeMe.replace("&lt;", "<");
        decodeMe.replace("&gt;", ">");
        return decodeMe;
    }
};

class stackoverflow : public LockedSingleton<stackoverflow>
{
public:
    vec<stackquery> queries;
    QString stackoverflowJsonPath;
    stackoverflow() { }
    ~stackoverflow() { serialize(); }
    stackoverflow(stackoverflow&)=delete;
    void operator=(stackoverflow&)=delete;
    void deserialize()
    {
        QFile f(stackoverflowJsonPath);
        if(!f.exists()) return;
        json jstackqueries=bot::fileRead(stackoverflowJsonPath);
        for(auto &jquery : jstackqueries)
        {
            QString channelId=j::unquote(jquery["forChannelId"]);
            QString txtquery=j::unquote(jquery["query"]);
            stackquery query(channelId,txtquery);
            query.resultIndex=j::to<quint64>(jquery["resultIndex"]);
            query.resultsPerMessage=j::to<quint64>(jquery["resultsPerMessage"]);
            query.messagesPerResponse=j::to<quint64>(jquery["messagesPerResponse"]);
            json jresults=jquery["results"];
            if(jresults.is_array())
                for(auto &jresult : jresults)
                {
                    query.results.push_back(stackresult(j::unquote(jresult["title"]),j::unquote(jresult["link"])));
                }
            query.refreshResults();
            queries.push_back(query);
        }
        qDebug() << "Read from:" << stackoverflowJsonPath << "size:" << jstackqueries.size();
    }
    void serialize()
    {
        if(stackoverflowJsonPath=="") return;
        json jstackqueries;
        for(auto &q : queries)
        {
            json jquery,jresults;
            jquery["resultIndex"]=q.resultIndex;
            jquery["forChannelId"]=q.channelId.toStdString();
            jquery["query"]=q.txtquery.toStdString();
            jquery["resultsPerMessage"]=q.resultsPerMessage;
            jquery["messagesPerResponse"]=q.messagesPerResponse;
            for(auto &r : q.results)
            {
                json jresult;
                jresult["title"]=r.title.toStdString();
                jresult["link"]=r.link.toStdString();
                jresults.push_back(jresult);
            }
            jquery["results"]=jresults;
            jstackqueries.push_back(jquery);
        }
        bot::fileWrite(j::toQString(jstackqueries).toUtf8(),stackoverflowJsonPath);
        qDebug() << "Wrote to:" << stackoverflowJsonPath << "size:" << jstackqueries.size();
    }
    void addOrUpdateQuery(QString channelId,stackquery searchquery)
    {
        bool updatedquery=false;
        for(size_t i=0; i < queries.size(); i++)
        {
            stackquery q=queries.at(i);
            if(q.channelId==channelId)
            {
                queries.at(i)=searchquery;
                updatedquery=true;
                break;
            }
        }
        if(!updatedquery)
            queries.push_back(searchquery);
    }
    void doSearch(QString channelId,QString txtquery)
    {
        stackquery searchquery(channelId,txtquery);
        searchquery.search();
        addOrUpdateQuery(channelId,searchquery);
    }
    inline static void setPath(QString path) { auto stack=get(); stack->stackoverflowJsonPath=path+"/stack.json"; stack->deserialize(); stack->release(); }
};

class discordbot : public QObject
{
    Q_OBJECT
signals:
    void setBotPath(QString path);
    void writeCompletedMessagesToDisk();
private:
    ptr<WebSocketService> websocket;
    v2p<SuspendableThread> processingthreads;
    discordusers discordUsers;
    QString botname,botkey,auth,host,api_url,get_gateway_url,websocket_url,websocket_url_action,post_message_url,is_typing_url,bot_path,userAgent,session_id,user_id,disc,avatar_url,avatar;
    QString updateStreamingStatusesOnChannel;
    QMutex mutex;
    quint64 version=7,resultLimit=5,msgLimit=1;
    int maxcharsperpost=2000,maxposts=7;
    bool tts=false;
public:
    discordbot(QString key) : botkey(key) { websocket=make<WebSocketService>(this,key); }
    ~discordbot() { SuspendableThread::setAllShouldStop(); writeCompletedMessagesToDisk(); Waiter w(2000); while(!websocket->messagesWritten && w.timeNotElapsed()) { } }
    void run()
    {
        connect(this,&discordbot::setBotPath,websocket.get(),&WebSocketService::setBotPath);
        connect(this,&discordbot::writeCompletedMessagesToDisk,websocket.get(),&WebSocketService::writeCompletedMessagesToDisk);
        auth="Bot "+botkey;
        host="discordapp.com";
        get_gateway_url="/api/gateway";
        websocket_url="wss://gateway.discord.gg";
        websocket_url_action=QString("/?v=%1&encoding=json").arg(version);
        api_url=QString("/api/v%1").arg(version);
        post_message_url=api_url+"/channels/%1/messages";
        is_typing_url=api_url+"/channels/%1/typing";
        if(!commands::commandsAdded())
        {
            setupCommands();
            commands::setCommandsAdded();
            qDebug() << "Commands are setup!";
        }
        initProcessingThreads();
        websocket->startService(websocket_url+websocket_url_action);
    }
    void initProcessingThreads()
    {
        for(qint64 i=0; i < bot_instances::thread_allowance; i++)
        {
            processingthreads.push_back(make<SuspendableThread>([&]
            {
                processNextMessageAndExecuteCommand();
            }));
        }
        bot *b=bot_instances::get()->botByKey(botkey);
        if(b)
        {
            bot_path=b->botPaths.botPath;
            userAgent=botname=b->botName;
            setBotPath(bot_path);
            discordUsers.botPaths=botpaths(bot_path);
            stackoverflow::setPath(bot_path);
        }
        bot_instances::release();
    }
    void suspend()
    {
        SuspendableThread::setAllSuspended();
        websocket->suspend();
        websocket->setUrl("");
    }
    void resume()
    {
        SuspendableThread::setAllResume();
        websocket->resume();
        websocket->setUrl(websocket_url+websocket_url_action);
    }
    void setWebClientConfigForDiscord(httpsclient &https)
    {
        https.setUseCompression(false);
        https.setAuthorization(auth);
        https.setUserAgent(userAgent);
    }
    void processNextMessageAndExecuteCommand()
    {
        QMutexLocker locker(&mutex);
        auto &messages=websocket->messages;
        if(messages->messagequeue.empty()) return;
        discordmessage &message=messages->messagequeue.front();
        for(auto &cmdarray : commands::get()->cmds)
        {
            for(auto &cmd : cmdarray.cmds)
            {
                if(cmd==message) //Message has a command to execute for it
                {
                    discordmessage messagecopy(message);
                    //Move message to completed array once handled both discord and user commands from message
                    if((message.hasUserCommand && message.handledUserCommand) || (!message.hasUserCommand && message.handledDiscordCommand))
                    {
                        messages->messagescompleted.push_back(messagecopy);
                        messages->messagequeue.erase(messages->messagequeue.begin());
                    }
                    locker.unlock();

                    //Execute command for message (a copy of it so other threads can continue processing messages while this command executes)
                    cmd.func(messagecopy);

                    if(messagecopy.handledDiscordCommand && !messagecopy.hasUserCommand)
                        qDebug() << "Executed matched discord command with cmd_id_hash:" << messagecopy.cmd_id_hash.toHex();
                    else if(messagecopy.handledUserCommand && messagecopy.hasUserCommand)
                        qDebug() << "Executed matched user command with usercmd_id_hash:" << messagecopy.usercmd_id_hash.toHex();
                    return;
                }
            }
        }
        //qDebug() << "Message not handled with cmd_id_hash:" << message.cmd_id_hash.toHex() << "nor usercmd_id_hash:" << message.usercmd_id_hash.toHex();
        //No command to execute exists for this message, move it to completed array anyway
        message.completed=3; //3 == Unhandled message, 2 == User command + Discord command completed, 1 == Discord command completed
        messages->messagescompleted.push_back(message);
        messages->messagequeue.erase(messages->messagequeue.begin());
    }
    void setupCommands()
    {
        //This way means no switch case blocks or if else blocks
        //Instead this is mapping message identifiers to function calls called commands

//Vital features like sending out a heartbeat, maintaining connection on and reconnecting the websocket,
//and handling invalid session and resuming, are now handled by the websocket service automatically.
//        commands::onDiscord("null",11,[&](discordmessage &msg) //Heartbeat acknowledgement
//        {
//            qDebug() << "Heartbeat acknowledged!";
//        });
//        commands::onDiscord("null",9,[&](discordmessage &msg) //Invalid session
//        {
//        });
//        commands::onDiscord("RESUMED",0,[&](discordmessage &msg) //Resumed
//        {
//        });

        //Handling discord messages
        commands::onDiscord("READY",0,[&](discordmessage &msg) //Ready state
        {
            session_id=j::unquote(msg.jsonmsg["d"]["session_id"]);
            user_id=j::unquote(msg.jsonmsg["d"]["user"]["id"]);
            disc=j::unquote(msg.jsonmsg["d"]["user"]["discriminator"]);
            avatar=j::unquote(msg.jsonmsg["d"]["user"]["avatar"]);
            QByteArray bot_user_avatar=updateBotAvatar();
            if(!bot_user_avatar.isEmpty())
                activitylogger::get()->setAvatar(bot_user_avatar);
            activitylogger::get()->setTitle(botname);
            qDebug() << "{READY} Received! : session_id:" << session_id<< "user_id:" << user_id+"#"+disc << "avatar:" << bot_user_avatar.size();
        });
        commands::onDiscord("GUILD_CREATE",0,[&](discordmessage &msg) //User message
        {
            json members=msg.jsonmsg["d"]["members"];
            for(auto &member : members)
            {
                discordUsers.addUserFromGuildCreate(member);
            }
            json presences=msg.jsonmsg["d"]["presences"];
            for(auto &presence : presences)
            {
                discordUsers.updateStatusForUser(presence);
            }
        });
        commands::onDiscord("MESSAGE_CREATE",0,[&](discordmessage &msg) //User message
        {
            auto user=discordUsers.addUserFromMessage(msg.jsonmsg);
            //if(msg.user.id != user_id)
            activitylogger::dbg(QString("%1: %2").arg(user.name).arg(msg.usermsg));
        });
        commands::onDiscord("PRESENCE_UPDATE",0,[&](discordmessage &msg) //User message
        {
            discordUsers.updateStatusForUser(msg.jsonmsg["d"]);
        });
        commands::onDiscord("VOICE_STATE_UPDATE",0,[&](discordmessage &msg)
        {
            discorduser u=discordUsers.updateStreamingStatusForUser(msg.jsonmsg["d"]);
            if(updateStreamingStatusesOnChannel=="") updateStreamingStatusesOnChannel=u.last_channel_id;
            if(u.isStreaming)
                sendTextMessage(updateStreamingStatusesOnChannel,QString("<@%1> just started streaming! Tune in now! :)").arg(u.id));
        });
//        commands::onDiscord("VOICE_STATE_UPDATE",0,[&](discordmessage &msg)
//        {
//            discorduser u=discordUsers.updateStreamingStatusForUser(msg.jsonmsg["d"]);
//            if(u.isStreaming)
//                sendTextMessage("809322058290429994",QString("<@%1> just went started streaming! Tune in now! :)").arg(u.id));
//        });

        //Handling custom user commands from messages received
        commands::onCustom("$btc",0,[&](discordmessage &msg)
        {
            CurrentBitcoinValue(msg);
        });
        commands::onCustom("$stack",0,[&](discordmessage &msg)
        {
            StackOverflowSearch(msg.channel_id,getCommandString(msg));
        });
        commands::onCustom("$stacknext",0,[&](discordmessage &msg)
        {
            auto stack=stackoverflow::get();
            for(auto &q : stack->queries)
            {
                if(q.channelId==msg.channel_id)
                {
                    q.resultIndex++;
                    sendTextMessage(msg.channel_id,j::escape(q.currentResultMessage()));
                    stack->release();
                    return;
                }
            }
            stack->release();
            sendTextMessage(msg.channel_id,"You haven't yet made a query yet... Type: $stack [search query] first!");
        });
        commands::onCustom("$stackconfig",0,[&](discordmessage &msg)
        {
            QStringList splitMsg=msg.usermsg.split(" ",Qt::SkipEmptyParts);
            for(int i=1; i < splitMsg.size()-1; i++)
            {
                if(splitMsg.at(i).toLower()=="results")
                    resultLimit=atoll(splitMsg.at(i+1).toStdString().c_str());
                else if(splitMsg.at(i).toLower()=="messages")
                    msgLimit=atoll(splitMsg.at(i+1).toStdString().c_str());
            }
            auto stack=stackoverflow::get();
            for(auto &q : stack->queries)
            {
                if(q.channelId==msg.channel_id)
                {
                    q.resultsPerMessage=resultLimit;
                    q.messagesPerResponse=msgLimit;
                    break;
                }
            }
            stack->release();
            sendTextMessage(msg.channel_id,QString().asprintf("New results per message limit: %llu and messages per response limit: %llu",resultLimit,msgLimit));
        });
        commands::onCustom("$git",0,[&](discordmessage &msg)
        {
            QString githuburl;
            auto gitcommand=msg.usermsg.split(" ",Qt::SkipEmptyParts);
            auto cmd=gitcommand.at(0);
            int numposts=4;
            bool forceEmbeddingCode=false;
            /*
             * Accept: $git [github_file_url] - Get file and post in up to 4 messages max or it uploads file as text
             *         $git[N] [github_file_url] - Get file and post in up to N messages max (does not upload file as text)
             *         $git [N] [github_file_url] - same as above
             *         (N / max posts clamped to 7)
             */
            if(!cmd.endsWith("t"))
            {
                QString N;
                QStringList Ns=cmd.split("t",Qt::SkipEmptyParts);
                if(Ns.size() > 1)
                    N=Ns.at(1);
                int value=N.toInt();
                if(value > 0)
                {
                    numposts=(value > maxposts ? maxposts : value);
                    forceEmbeddingCode=true;
                }
            }
            if(gitcommand.at(1).startsWith("https://"))
            {
                githuburl=gitcommand.at(1);
            }
            else if(gitcommand.at(2).startsWith("https://"))
            {
                githuburl=gitcommand.at(2);
                int value=gitcommand.at(1).toInt();
                if(value > 0)
                {
                    numposts=(value > maxposts ? maxposts : value);
                    forceEmbeddingCode=true;
                }
            }
            qDebug() << "githuburl:" << githuburl;
            GithubGetAndPost(githuburl,msg.channel_id,numposts,forceEmbeddingCode);
        });
    }
    QByteArray updateBotAvatar()
    {
        QByteArray botavatar;
        bot *b=bot_instances::get()->botByKey(botkey);
        if(b)
        {
            if(b->bot_avatar.isEmpty())
            {
                httpsclient https;
                setWebClientConfigForDiscord(https);
                avatar_url=bot_instances::avatarsUrl.arg(user_id).arg(avatar);
                auto response=https.send(httpsrequest(httpsrequest::type::GET,avatar_url));
                if(response.content.size() > 0)
                    botavatar=b->bot_avatar=response.content;
                else
                    botavatar=b->bot_avatar=bot::fileRead(b->botPaths.avatarPath);
            }
            else
                botavatar=b->bot_avatar=bot::fileRead(b->botPaths.avatarPath);
        }
        bot_instances::release();
        return botavatar;
    }
    QString getCommandString(discordmessage &msg)
    {
        //Get 'command string' starting from second index after space(s) and then remove any further extra spaces
        QStringList commandAsList=msg.usermsg.split(" ",Qt::SkipEmptyParts);
        QString commandString;
        for(int i=1; i < commandAsList.size(); i++)
        {
            commandString+=commandAsList.at(i);
            if(i != (commandAsList.size()-1))
                commandString+=" ";
        }
        return commandString;
    }
    void getGateway()
    {
        httpsclient https;
        setWebClientConfigForDiscord(https);
        auto response=https.send(httpsrequest::get("https://"+host+get_gateway_url));
        if(response.success)
        {
            qDebug() << "Websocket gateway requested! Response:" << response.text;
            json js=j::fromQString(response.content);
            QString new_websocket_url=j::unquote(js["url"]);
            if(bot::isNotEmptyOrNull(new_websocket_url))
                websocket_url=new_websocket_url;
            qDebug() << "Websocket Url:" << websocket_url;
        }
    }
    bool sendIsTyping(QString channelId)
    {
        if(bot::isEmptyOrNull(channelId)) return false;
        httpsclient https;
        setWebClientConfigForDiscord(https);
        auto response=https.send(httpsrequest::post("https://"+host+is_typing_url.arg(channelId)));
        return response.success;
    }
    bool sendTextMessage(const QString channelId, const QString message, const QString embeddedTitle="", const QString embeddedMessage="")
    {
        if(bot::isEmptyOrNull(channelId)) return false;
        httpsclient https;
        setWebClientConfigForDiscord(https);
        QString jsonString;
        if(embeddedMessage.isEmpty())
        {
            if(bot::isEmptyOrNull(message)) return false;
            jsonString=QString(R"({"content":"%1","tts":%2})").arg(message).arg(tts);
        }
        else
        {
            jsonString=QString(R"({"content":"%1","tts":%2,"embed":{"title":"%3","description":"%4"}})").arg(message).arg(tts).arg(embeddedTitle).arg(embeddedMessage);
        }

        auto response=https.send(httpsrequest::post("https://"+host+post_message_url.arg(channelId),jsonString));
        return response.success;
    }

    bool sendTextFile(QString channelId,QString filename,QByteArray textfile,QString message="")
    {
        httpsclient https;
        setWebClientConfigForDiscord(https);
        auto request=httpsrequest::post("https://"+host+post_message_url.arg(channelId));
        QByteArray payload_json=QString("{\"tts\":false,\"content\":\"%1\"}").arg(message).toUtf8();
        request.addMultiPartData(httpsrequest::multipart::TEXT,"payload_json",payload_json);
        request.addMultiPartData(httpsrequest::multipart::OCTET_STREAM,filename,textfile);

        auto response=https.send(request);
        return response.success;
    }

    bool sendImageFile(QString channelId,QString filename,QByteArray imgfile,QString message="")
    {
        httpsclient https;
        setWebClientConfigForDiscord(https);
        auto request=httpsrequest::post("https://"+host+post_message_url.arg(channelId));
        QByteArray payload_json=QString("{\"tts\":%1,\"content\":\"%2\"}").arg(tts).arg(message).toUtf8();
        request.addMultiPartData(httpsrequest::multipart::TEXT,"payload_json",payload_json);
        QString fileExtension=bot::getFileExtension(filename);
        if(fileExtension.toLower()==".png")
            request.addMultiPartData(httpsrequest::multipart::PNG,filename,imgfile);
        else if(fileExtension.toLower()==".jpg" || fileExtension.toLower()==".jpeg")
            request.addMultiPartData(httpsrequest::multipart::JPG,filename,imgfile);
        else if(fileExtension.toLower()==".gif")
            request.addMultiPartData(httpsrequest::multipart::GIF,filename,imgfile);
        else
            request.addMultiPartData(httpsrequest::multipart::OCTET_STREAM,filename,imgfile);

        auto response=https.send(request);
        return response.success;
    }

    bool sendImageFromFile(QString channelId,QString filename,QString filepath,QString message="")
    {
        bool success=false;
        QByteArray imgfile=bot::fileRead(filepath,&success);
        if(!success) return false;
        return sendImageFile(channelId,filename,imgfile,message);
    }

    bool messageWrap(QString channel_id,QString rawmessage,int maxposts=4)
    {
        int remainingActualBytes=rawmessage.size();
        for(int i=0,x=0,remainingPossibleBytes=((maxcharsperpost * maxposts)); remainingPossibleBytes > 0; i+=maxcharsperpost,remainingPossibleBytes-=maxcharsperpost,x++)
        {
            if(remainingPossibleBytes <= 0)
            {
                qDebug() << "Wrapped message into" << x << "posts...";
                if(remainingActualBytes > 0)
                    qDebug() << remainingActualBytes << "bytes more of the message not posted though...";
                return true;
            }
            sendTextMessage(channel_id,rawmessage.mid(i,maxcharsperpost));
        }
        return false;
    }

    bool codeWrap(QString channel_id,QString escapedcode,QString sourcefilename,int maxposts=4,bool forceEmbeddingCodeInMessages=false)
    {
        QString codewrapbegin,codewrapend=j::escape("\n```");
        QString sourcefileextension=bot::getFileExtension(sourcefilename);
        if(sourcefileextension==".cpp" || sourcefileextension==".h" || sourcefileextension==".hpp" || sourcefileextension==".zig")
            codewrapbegin=j::escape("```cpp\n");
        else if(sourcefileextension==".c")
            codewrapbegin=j::escape("```c\n");
        else if(sourcefileextension==".m" || sourcefileextension==".cc")
            codewrapbegin=j::escape("```objective-c\n");
        else if(sourcefileextension==".js")
            codewrapbegin=j::escape("```javascript\n");
        else if(sourcefileextension==".css")
            codewrapbegin=j::escape("```css\n");
        else if(sourcefileextension==".py")
            codewrapbegin=j::escape("```python\n");
        else if(sourcefileextension==".java")
            codewrapbegin=j::escape("```java\n");
        else if(sourcefileextension==".php")
            codewrapbegin=j::escape("```php\n");
        int headerfootersize=codewrapbegin.size()+codewrapend.size();
        if(forceEmbeddingCodeInMessages)
        {
            int remainingActualBytes=escapedcode.size();
            for(int i=0,r=(maxcharsperpost*maxposts)-(headerfootersize*maxposts); r > 0; i+=maxcharsperpost,r-=maxcharsperpost)
            {
                if(r <= 0)
                {
                    int x=i/maxcharsperpost;
                    qDebug() << "Wrapped code into" << x << "posts...";
                    if(remainingActualBytes > 0)
                        qDebug() << remainingActualBytes << "bytes more of the code not posted though...";
                    return true;
                }
                sendTextMessage(channel_id,codewrapbegin+escapedcode.mid(i,maxcharsperpost)+codewrapend);
            }
        }
        else
        {
            if(escapedcode.size() <= ((maxcharsperpost * maxposts) - (headerfootersize * maxposts)))
            {
                for(int i=0, remainingbytes=escapedcode.size(); remainingbytes > 0; i+=maxcharsperpost,remainingbytes-=maxcharsperpost)
                {
                    sendTextMessage(channel_id,codewrapbegin+escapedcode.mid(i,maxcharsperpost)+codewrapend);
                }
                return true;
            }
            else
            {
                sendTextFile(channel_id,sourcefilename,escapedcode.toUtf8(),"Hey I got this for you! Thought you might like it :)");
            }
        }
        return false;
    }

    void GithubGetAndPost(QString githuburl,QString channel_id,int numposts=4,bool forceEmbeddingCodeInMessages=false)
    {
        if(githuburl.startsWith("https://github.com"))
        {
            //Fix regular github url into raw github url
            githuburl.replace("github.com","raw.githubusercontent.com");
            githuburl.replace("/blob","");
            qDebug() << "raw githuburl:" << githuburl;
        }
        if(!githuburl.startsWith("https://raw.githubusercontent.com"))
        {
            sendTextMessage(channel_id,"Invalid github file url! This only will get and post/return a file from a github repository...");
            return;
        }
        httpsclient https;
        auto response=https.send(httpsrequest::get(githuburl));
        if(response.success)
        {
            QString code=j::escape(response.content),sourcefilename=bot::getFileName(githuburl);
            codeWrap(channel_id,code,sourcefilename,numposts,forceEmbeddingCodeInMessages);
            qDebug() << "Posted github file:" << githuburl << "\r\nfile name:" << sourcefilename << "file size:" << response.content.size();
        }
    }
    void StackOverflowSearch(QString channelId,QString txtquery)
    {
        qDebug() << "Stack txtquery:" << txtquery;
        if(channelId.isEmpty() || txtquery.isEmpty()) return;
        sendIsTyping(channelId);
        stackoverflow::get()->doSearch(channelId,txtquery);
        stackoverflow::release();
    }
    void CurrentBitcoinValue(discordmessage &msg)
    {
        QString amountOfBtcString=getCommandString(msg);
        if(amountOfBtcString.isEmpty())
            amountOfBtcString="1";
        else if(amountOfBtcString.contains(" "))
            amountOfBtcString.truncate(amountOfBtcString.indexOf(" "));
        if((quint64)amountOfBtcString.toDouble()==0)
            amountOfBtcString="1337.13371337";
        qDebug() << "amountOfBtc:" << amountOfBtcString;
        httpsclient https;
        auto response=https.send(httpsrequest::get("http://preev.com/pulse/units:btc+usd/sources:bitstamp+kraken"));
        if(response.success)
        {
            json j=j::fromQString(response.content);
            QString btcusd1=j::unquote(j["btc"]["usd"]["bitstamp"]["last"]);
            QString btcusd2=j::unquote(j["btc"]["usd"]["kraken"]["last"]);
            QString btcusd,btcTruncated;
            int i=QRandomGenerator().bounded(0,1);
            if(i==0) btcusd=btcusd1; else btcusd=btcusd2;
            QStringList places=amountOfBtcString.split(".",Qt::SkipEmptyParts);
            quint64 satoshis=0;
            vec<quint64> satoshisConversion={100000000,10000000,1000000,100000,10000,1000,100,10,1};
            if(places.size() > 0)
            {
                quint64 firstDigit=places.at(0).toULongLong();
                satoshis=firstDigit*satoshisConversion[0];
                if(places.size() > 1)
                {
                    btcTruncated=places.at(1);
                    if(btcTruncated.size() > 8)
                        btcTruncated.resize(8);
                    QByteArray p=btcTruncated.toUtf8();
                    for(size_t i=0; i < (size_t)p.size(); i++)
                    {
                        char value[2]{0};
                        value[0]=p.data()[i];
                        quint64 z=QString(value).toULongLong();
                        qDebug() << z << "*" << satoshisConversion[i+1] << "=" << z*satoshisConversion[i+1];
                        satoshis+=z*satoshisConversion[i+1];
                    }
                }
            }
            if(places.size()==1)
                btcTruncated=places.at(0)+".0";
            else
                btcTruncated=places.at(0)+"."+btcTruncated;
            qDebug() << btcTruncated << "==" << satoshis << "satoshis! :P";
            quint64 btcusdvalue=(quint64)btcusd.toDouble();
            quint64 satoshisusdvalue=(satoshis*btcusdvalue);
            quint64 usdvalue=(satoshisusdvalue/satoshisConversion[0]);
            qDebug() << "btcusdvalue:" << btcusdvalue << "satoshisusd value:" << satoshisusdvalue << "usdvalue:" << usdvalue;
            QString satoshisString=QString("%1").arg(satoshis);
            QString usdString=QString("%1").arg(usdvalue);
            sendTextMessage(msg.channel_id,"Current price:","BTCUSD",QString("BTC %1 == %2 satoshis == $%3")
                            .arg(bot::prettifyMoneyString(btcTruncated)).arg(bot::prettifyMoneyString(satoshisString)).arg(bot::prettifyMoneyString(usdString)));
        }
    }
public slots:
    void sslErrors(const QList<QSslError> &errors)
    {
        for(auto &e : errors)
        {
            if(e.errorString().indexOf("already connecting/connected") != -1)
                qDebug() << "already connected...";
            qDebug() << "error:" << e.error() << "error string:" << e.errorString();
        }
    }
};
class botcommander : public QObject
{
    Q_OBJECT
public:
    inline static ptr<botcommander> commander=nullptr;
    v2p<discordbot> bots;

    botcommander()
    {
        run();
        activitylogger::dbg(QString("%1 discord bot instances launched!").arg(bots.size()));
    }
    ~botcommander()
    {
        size_t numbots=bots.size();
        bots.clear();
        bot_instances::free();
        activitylogger::dbg(QString("%1 discord bot instances stopped...").arg(numbots));
    }
    botcommander(botcommander&)=delete;
    void operator=(botcommander&)=delete;
    void run()
    {
        auto instances=bot_instances::get();
        for(auto &instance : instances->bots)
        {
           if(instance->botKey=="") continue;
           bots.push_back(make<discordbot>(instance->botKey));
           bots.back()->run();
        }
        instances->release();
        QRandomGenerator().seed(QDateTime::currentSecsSinceEpoch());
    }
    inline static void start() { if(commander==nullptr) commander=make<botcommander>(); }
    inline static void stop() { commander.reset(); }
};

#endif // DISCORDBOT_H
