/****************************************************************************
** Meta object code from reading C++ file 'fvupdater.h'
**
** Created: Sat 3. Nov 19:25:47 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../updater/fvupdater.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fvupdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FvUpdater[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,
      54,   10,   10,   10, 0x05,
      75,   66,   10,   10, 0x05,
      90,   10,   10,   10, 0x05,
     105,  101,   10,   10, 0x05,
     137,  121,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     199,  174,  169,   10, 0x0a,
     221,   10,  169,   10, 0x2a,
     239,   10,   10,   10, 0x0a,
     255,   10,   10,   10, 0x0a,
     268,   10,   10,   10, 0x0a,
     284,   10,   10,   10, 0x0a,
     299,   10,  169,   10, 0x0a,
     323,   10,  169,   10, 0x0a,
     352,  350,   10,   10, 0x09,
     387,  350,   10,   10, 0x09,
     427,  424,   10,   10, 0x09,
     470,   10,   10,   10, 0x08,
     511,  490,   10,   10, 0x08,
     557,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FvUpdater[] = {
    "FvUpdater\0\0update\0updateAvailable(FvAvailableUpdate*)\0"
    "noUpdates()\0percents\0progress(uint)\0"
    "finished()\0msg\0failed(QString)\0"
    "pathToInstaller\0closeAppToRunInstaller(QString)\0"
    "bool\0silentAsMuchAsItCouldGet\0"
    "CheckForUpdates(bool)\0CheckForUpdates()\0"
    "InstallUpdate()\0SkipUpdate()\0"
    "RemindMeLater()\0CancelUpdate()\0"
    "CheckForUpdatesSilent()\0"
    "CheckForUpdatesNotSilent()\0,\0"
    "downloadUpdateFailed(QUrl,QString)\0"
    "downloadUpdateFinished(QUrl,QString)\0"
    ",,\0downloadUpdateProgress(QUrl,qint64,qint64)\0"
    "httpFeedReadyRead()\0bytesRead,totalBytes\0"
    "httpFeedUpdateDataReadProgress(qint64,qint64)\0"
    "httpFeedDownloadFinished()\0"
};

void FvUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FvUpdater *_t = static_cast<FvUpdater *>(_o);
        switch (_id) {
        case 0: _t->updateAvailable((*reinterpret_cast< FvAvailableUpdate*(*)>(_a[1]))); break;
        case 1: _t->noUpdates(); break;
        case 2: _t->progress((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 3: _t->finished(); break;
        case 4: _t->failed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->closeAppToRunInstaller((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: { bool _r = _t->CheckForUpdates((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = _t->CheckForUpdates();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: _t->InstallUpdate(); break;
        case 9: _t->SkipUpdate(); break;
        case 10: _t->RemindMeLater(); break;
        case 11: _t->CancelUpdate(); break;
        case 12: { bool _r = _t->CheckForUpdatesSilent();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->CheckForUpdatesNotSilent();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: _t->downloadUpdateFailed((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 15: _t->downloadUpdateFinished((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 16: _t->downloadUpdateProgress((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 17: _t->httpFeedReadyRead(); break;
        case 18: _t->httpFeedUpdateDataReadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 19: _t->httpFeedDownloadFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FvUpdater::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FvUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FvUpdater,
      qt_meta_data_FvUpdater, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FvUpdater::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FvUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FvUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FvUpdater))
        return static_cast<void*>(const_cast< FvUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int FvUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void FvUpdater::updateAvailable(FvAvailableUpdate * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FvUpdater::noUpdates()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void FvUpdater::progress(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FvUpdater::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void FvUpdater::failed(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void FvUpdater::closeAppToRunInstaller(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
