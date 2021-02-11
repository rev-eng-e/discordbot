/****************************************************************************
** Meta object code from reading C++ file 'discordbot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../discordbot/discordbot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'discordbot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_activitylogger_t {
    QByteArrayData data[16];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_activitylogger_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_activitylogger_t qt_meta_stringdata_activitylogger = {
    {
QT_MOC_LITERAL(0, 0, 14), // "activitylogger"
QT_MOC_LITERAL(1, 15, 3), // "log"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "text"
QT_MOC_LITERAL(4, 25, 9), // "setAvatar"
QT_MOC_LITERAL(5, 35, 11), // "avatarImage"
QT_MOC_LITERAL(6, 47, 8), // "setTitle"
QT_MOC_LITERAL(7, 56, 5), // "title"
QT_MOC_LITERAL(8, 62, 10), // "setBotName"
QT_MOC_LITERAL(9, 73, 7), // "botname"
QT_MOC_LITERAL(10, 81, 9), // "setBotKey"
QT_MOC_LITERAL(11, 91, 6), // "botkey"
QT_MOC_LITERAL(12, 98, 33), // "activateMessagePlaybackForBot..."
QT_MOC_LITERAL(13, 132, 7), // "botpath"
QT_MOC_LITERAL(14, 140, 15), // "addBotInstances"
QT_MOC_LITERAL(15, 156, 13) // "instanceNames"

    },
    "activitylogger\0log\0\0text\0setAvatar\0"
    "avatarImage\0setTitle\0title\0setBotName\0"
    "botname\0setBotKey\0botkey\0"
    "activateMessagePlaybackForBotPath\0"
    "botpath\0addBotInstances\0instanceNames"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_activitylogger[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,
       8,    1,   58,    2, 0x06 /* Public */,
      10,    1,   61,    2, 0x06 /* Public */,
      12,    1,   64,    2, 0x06 /* Public */,
      14,    1,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QStringList,   15,

       0        // eod
};

void activitylogger::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<activitylogger *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setAvatar((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 2: _t->setTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->setBotName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->setBotKey((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->activateMessagePlaybackForBotPath((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->addBotInstances((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (activitylogger::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::log)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::setAvatar)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::setTitle)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::setBotName)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::setBotKey)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::activateMessagePlaybackForBotPath)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (activitylogger::*)(const QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&activitylogger::addBotInstances)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject activitylogger::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_activitylogger.data,
    qt_meta_data_activitylogger,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *activitylogger::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *activitylogger::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_activitylogger.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int activitylogger::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void activitylogger::log(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void activitylogger::setAvatar(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void activitylogger::setTitle(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void activitylogger::setBotName(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void activitylogger::setBotKey(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void activitylogger::activateMessagePlaybackForBotPath(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void activitylogger::addBotInstances(const QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
struct qt_meta_stringdata_messagestorage_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_messagestorage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_messagestorage_t qt_meta_stringdata_messagestorage = {
    {
QT_MOC_LITERAL(0, 0, 14) // "messagestorage"

    },
    "messagestorage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_messagestorage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void messagestorage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject messagestorage::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_messagestorage.data,
    qt_meta_data_messagestorage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *messagestorage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *messagestorage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_messagestorage.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int messagestorage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_WebSocketService_t {
    QByteArrayData data[8];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WebSocketService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WebSocketService_t qt_meta_stringdata_WebSocketService = {
    {
QT_MOC_LITERAL(0, 0, 16), // "WebSocketService"
QT_MOC_LITERAL(1, 17, 14), // "openConnection"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 3), // "url"
QT_MOC_LITERAL(4, 37, 15), // "sendTextMessage"
QT_MOC_LITERAL(5, 53, 7), // "message"
QT_MOC_LITERAL(6, 61, 15), // "closeConnection"
QT_MOC_LITERAL(7, 77, 19) // "textMessageReceived"

    },
    "WebSocketService\0openConnection\0\0url\0"
    "sendTextMessage\0message\0closeConnection\0"
    "textMessageReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WebSocketService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       6,    0,   40,    2, 0x06 /* Public */,
       7,    1,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::LongLong, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,

       0        // eod
};

void WebSocketService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WebSocketService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openConnection((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: { qint64 _r = _t->sendTextMessage((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->closeConnection(); break;
        case 3: _t->textMessageReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WebSocketService::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WebSocketService::openConnection)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = qint64 (WebSocketService::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WebSocketService::sendTextMessage)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WebSocketService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WebSocketService::closeConnection)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WebSocketService::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WebSocketService::textMessageReceived)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WebSocketService::staticMetaObject = { {
    QMetaObject::SuperData::link<Thread<WebSocketWorker>::staticMetaObject>(),
    qt_meta_stringdata_WebSocketService.data,
    qt_meta_data_WebSocketService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WebSocketService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WebSocketService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WebSocketService.stringdata0))
        return static_cast<void*>(this);
    return Thread<WebSocketWorker>::qt_metacast(_clname);
}

int WebSocketService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Thread<WebSocketWorker>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void WebSocketService::openConnection(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
qint64 WebSocketService::sendTextMessage(const QString & _t1)
{
    qint64 _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}

// SIGNAL 2
void WebSocketService::closeConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void WebSocketService::textMessageReceived(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_discordbot_t {
    QByteArrayData data[8];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_discordbot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_discordbot_t qt_meta_stringdata_discordbot = {
    {
QT_MOC_LITERAL(0, 0, 10), // "discordbot"
QT_MOC_LITERAL(1, 11, 10), // "setBotPath"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "path"
QT_MOC_LITERAL(4, 28, 28), // "writeCompletedMessagesToDisk"
QT_MOC_LITERAL(5, 57, 9), // "sslErrors"
QT_MOC_LITERAL(6, 67, 16), // "QList<QSslError>"
QT_MOC_LITERAL(7, 84, 6) // "errors"

    },
    "discordbot\0setBotPath\0\0path\0"
    "writeCompletedMessagesToDisk\0sslErrors\0"
    "QList<QSslError>\0errors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_discordbot[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    0,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void discordbot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<discordbot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setBotPath((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->writeCompletedMessagesToDisk(); break;
        case 2: _t->sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (discordbot::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&discordbot::setBotPath)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (discordbot::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&discordbot::writeCompletedMessagesToDisk)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject discordbot::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_discordbot.data,
    qt_meta_data_discordbot,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *discordbot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *discordbot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_discordbot.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int discordbot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void discordbot::setBotPath(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void discordbot::writeCompletedMessagesToDisk()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_botcommander_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_botcommander_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_botcommander_t qt_meta_stringdata_botcommander = {
    {
QT_MOC_LITERAL(0, 0, 12) // "botcommander"

    },
    "botcommander"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_botcommander[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void botcommander::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject botcommander::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_botcommander.data,
    qt_meta_data_botcommander,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *botcommander::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *botcommander::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_botcommander.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int botcommander::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
