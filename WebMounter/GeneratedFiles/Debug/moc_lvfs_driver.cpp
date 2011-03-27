/****************************************************************************
** Meta object code from reading C++ file 'lvfs_driver.h'
**
** Created: Sun 27. Mar 21:28:12 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../lvfs_driver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lvfs_driver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LocalDriver__ILVFSDriver[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,
      36,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   48,   25,   25, 0x0a,
      94,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LocalDriver__ILVFSDriver[] = {
    "LocalDriver::ILVFSDriver\0\0mounted()\0"
    "unmounted()\0generalSettings\0"
    "mount(Data::GeneralSettings&)\0unmount()\0"
};

const QMetaObject LocalDriver::ILVFSDriver::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_LocalDriver__ILVFSDriver,
      qt_meta_data_LocalDriver__ILVFSDriver, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LocalDriver::ILVFSDriver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LocalDriver::ILVFSDriver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LocalDriver::ILVFSDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LocalDriver__ILVFSDriver))
        return static_cast<void*>(const_cast< ILVFSDriver*>(this));
    return QThread::qt_metacast(_clname);
}

int LocalDriver::ILVFSDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: mounted(); break;
        case 1: unmounted(); break;
        case 2: mount((*reinterpret_cast< Data::GeneralSettings(*)>(_a[1]))); break;
        case 3: unmount(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void LocalDriver::ILVFSDriver::mounted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void LocalDriver::ILVFSDriver::unmounted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
