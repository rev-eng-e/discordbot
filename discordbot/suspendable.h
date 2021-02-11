#ifndef SUSPENDABLE_H
#define SUSPENDABLE_H

#include <QWebSocket>
#include <QPointer>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

//Thank you Andrei Smirnov!
//SuspendableWorker and Thread are for managing threads of objects that require signals/slots working (requring a QEventLoop)
class SuspendableWorker : public QObject
{
    Q_OBJECT
private:
    QMutex waitMutex;
    QWaitCondition waitCondition;
private slots:
    void suspendImpl()
    {
        waitCondition.wait(&waitMutex);
    }
public:
    std::atomic<bool> suspended=false;
    explicit SuspendableWorker(QObject *parent=nullptr) : QObject(parent)
    {
        waitMutex.lock();
    }
    ~SuspendableWorker()
    {
        waitCondition.wakeAll();
        waitMutex.unlock();
    }
    // resume() must be called from the outer thread.
    void resume()
    {
        suspended=false;
        waitCondition.wakeAll();
    }
    // suspend() must be called from the outer thread.
    // the function would block the caller's thread until
    // the worker thread is suspended.
    void suspend()
    {
        suspended=true;
        QMetaObject::invokeMethod(this, &SuspendableWorker::suspendImpl);
        // acquiring mutex to block the calling thread
        waitMutex.lock();
        waitMutex.unlock();
    }
};

class WebSocketWorker : public SuspendableWorker
{
    Q_OBJECT
public:
    QPointer<QWebSocket> wss=nullptr;
    std::atomic<bool> connected=false;
    std::atomic<uint64_t> connectionattempts=0,connectionfailures=0;
    WebSocketWorker(QObject *parent=nullptr) : SuspendableWorker(parent) { }
signals:
    void textMessageReceived(QString message);
public slots:
    void onOpenConnection(QString url)
    {
        if(wss==nullptr)
            wss=new QWebSocket();
        //Signals to slots
        connect(wss,&QWebSocket::connected,this,&WebSocketWorker::onWebSocketConnected,Qt::UniqueConnection);
        connect(wss,&QWebSocket::disconnected,this,&WebSocketWorker::onWebSocketDisconnected,Qt::UniqueConnection);
        connect(wss,QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),this,&WebSocketWorker::onWebSocketSslErrors,Qt::UniqueConnection);
        //Signal forwarding
        connect(wss,&QWebSocket::textMessageReceived,this,&WebSocketWorker::textMessageReceived,Qt::UniqueConnection);
        connectionattempts++;
        wss->open(QUrl(url));
    }
    qint64 onSendTextMessage(const QString &message)
    {
        return wss->sendTextMessage(message);
    }
    void onCloseConnection()
    {
        if(connected) wss->close();
    }
    void onWebSocketConnected()
    {
        connectionfailures=connectionattempts=0;
        connected=true;
        qDebug() << wss << "connected! on thread:" << QThread::currentThread();
    }
    void onWebSocketDisconnected()
    {
        connected=false;
        qDebug() << wss << "connection lost on thread:" << QThread::currentThread();
    }
    void onWebSocketSslErrors(const QList<QSslError> &errors)
    {
        for(auto &e : errors)
        {
            if(e.errorString().indexOf("already connecting/connected") != -1)
                qDebug() << "already connected...";
            qDebug() << "error:" << e.error() << "error string:" << e.errorString();
        }
    }
};

template <typename TWorker> class Thread : public QThread
{
public:
    TWorker *worker;
    explicit Thread(TWorker *_worker,QObject *parent=nullptr) : QThread(parent),worker(_worker)
    {
        worker->moveToThread(this);
        start();
    }
    ~Thread()
    {
        resume();
        quit();
        wait();
    }
    void suspend()
    {
        auto suspendableworker=qobject_cast<SuspendableWorker*>(worker);
        if (suspendableworker!=nullptr)
            suspendableworker->suspend();
    }
    void resume()
    {
        auto suspendableworker=qobject_cast<SuspendableWorker*>(worker);
        if (suspendableworker!=nullptr)
            suspendableworker->resume();
    }
protected:
    void run() override
    {
        QThread::run();
        delete worker;
    }
};

//SuspendableThread is for creating auto starting suspendable threads when no signals/slots are required for the thread
class SuspendableThread : public QThread
{
private:
    inline static std::atomic<bool> allthreadsshouldstop=false,allthreadssuspended=false;
    std::atomic<bool> threadrunning=false,threadsuspended=false,threadshouldstop=false,shouldrunonce=false;
    std::function<void()> executefunction;
public:
    explicit SuspendableThread(std::function<void()> func,bool runonce=false,QObject *parent=nullptr) : QThread(parent)
    {
        executefunction=func;
        shouldrunonce=runonce;
        start();
    }
    ~SuspendableThread()
    {
        stop();
    }
    inline static void setAllShouldStop(bool shouldstop=true) { allthreadsshouldstop=shouldstop; }
    inline static bool shouldAllStop() { return allthreadsshouldstop; }
    inline static void setAllSuspended(bool allsuspended=true) { allthreadssuspended=allsuspended; }
    inline static void setAllResume(bool allresume=true) { allthreadssuspended=!allresume; }
    void setShouldStop(bool shouldstop=true) { threadshouldstop=shouldstop; }
    bool shouldStop() { return threadshouldstop; }
    void stop() { setShouldStop(); while(threadrunning) { msleep(1); } }
    void suspend() { threadsuspended=true; }
    void resume() { threadsuspended=false; }
protected:
    void run() override
    {
        threadrunning=true;
        qDebug() << QThread::currentThread() << "started!";
        if(shouldrunonce)
            executefunction();
        else
        {
            forever
            {
                if(threadshouldstop || allthreadsshouldstop)
                    break;
                if(!threadsuspended && !allthreadssuspended)
                    executefunction();
                else
                    msleep(1);
            }
        }
        threadrunning=threadsuspended=threadshouldstop=false;
        qDebug() << QThread::currentThread() << "exited!";
    }
};

//This is a non-blocking time delayer for threads
class Waiter : QObject
{
    Q_OBJECT
public:
    qint64 waitUntilTime=0,sleepTime=1;
    Waiter(qint64 delay=1000)
    {
        waitUntilTime=QDateTime::currentMSecsSinceEpoch()+delay;
    }
    bool timeNotElapsed()
    {
        QThread::msleep(sleepTime);
        return waitUntilTime > QDateTime::currentMSecsSinceEpoch();
    }
    void wait(std::function<void()> func=[](){})
    {
        while(waitUntilTime > QDateTime::currentMSecsSinceEpoch())
        {
            func();
            QThread::msleep(sleepTime);
        }
    }
};

#endif // SUSPENDABLE_H
