/****************************************************************************
** Meta object code from reading C++ file 'rvfs_driver.h'
**
** Created: Sat 3. Nov 19:27:19 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../rvfs_driver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rvfs_driver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RemoteDriver__RVFSDriver[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   26,   25,   25, 0x05,
      48,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   25,   25,   25, 0x0a,
      98,   25,   25,   25, 0x0a,
     111,   25,   25,   25, 0x0a,
     123,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RemoteDriver__RVFSDriver[] = {
    "RemoteDriver::RVFSDriver\0\0,\0"
    "updateView(int,int)\0fileUploaded()\0"
    "startPlugin(Data::PluginSettings&)\0"
    "stopPlugin()\0startSync()\0stopSync()\0"
};

void RemoteDriver::RVFSDriver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RVFSDriver *_t = static_cast<RVFSDriver *>(_o);
        switch (_id) {
        case 0: _t->updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->fileUploaded(); break;
        case 2: _t->startPlugin((*reinterpret_cast< Data::PluginSettings(*)>(_a[1]))); break;
        case 3: _t->stopPlugin(); break;
        case 4: _t->startSync(); break;
        case 5: _t->stopSync(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RemoteDriver::RVFSDriver::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RemoteDriver::RVFSDriver::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_RemoteDriver__RVFSDriver,
      qt_meta_data_RemoteDriver__RVFSDriver, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RemoteDriver::RVFSDriver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RemoteDriver::RVFSDriver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RemoteDriver::RVFSDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RemoteDriver__RVFSDriver))
        return static_cast<void*>(const_cast< RVFSDriver*>(this));
    return QThread::qt_metacast(_clname);
}

int RemoteDriver::RVFSDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void RemoteDriver::RVFSDriver::updateView(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RemoteDriver::RVFSDriver::fileUploaded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
