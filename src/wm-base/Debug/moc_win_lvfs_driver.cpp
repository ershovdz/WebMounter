/****************************************************************************
** Meta object code from reading C++ file 'win_lvfs_driver.h'
**
** Created: Sat 3. Nov 19:27:19 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../win_lvfs_driver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_lvfs_driver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LocalDriver__LVFSDriver[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      41,   25,   24,   24, 0x0a,
      71,   24,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LocalDriver__LVFSDriver[] = {
    "LocalDriver::LVFSDriver\0\0generalSettings\0"
    "mount(Data::GeneralSettings&)\0unmount()\0"
};

void LocalDriver::LVFSDriver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LVFSDriver *_t = static_cast<LVFSDriver *>(_o);
        switch (_id) {
        case 0: _t->mount((*reinterpret_cast< Data::GeneralSettings(*)>(_a[1]))); break;
        case 1: _t->unmount(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LocalDriver::LVFSDriver::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LocalDriver::LVFSDriver::staticMetaObject = {
    { &ILVFSDriver::staticMetaObject, qt_meta_stringdata_LocalDriver__LVFSDriver,
      qt_meta_data_LocalDriver__LVFSDriver, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LocalDriver::LVFSDriver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LocalDriver::LVFSDriver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LocalDriver::LVFSDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LocalDriver__LVFSDriver))
        return static_cast<void*>(const_cast< LVFSDriver*>(this));
    return ILVFSDriver::qt_metacast(_clname);
}

int LocalDriver::LVFSDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ILVFSDriver::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
