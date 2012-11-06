/****************************************************************************
** Meta object code from reading C++ file 'webmounter.h'
**
** Created: Tue 6. Nov 23:05:47 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../webmounter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'webmounter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Common__WebMounter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      28,   19,   19,   19, 0x0a,
      38,   19,   19,   19, 0x08,
      48,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Common__WebMounter[] = {
    "Common::WebMounter\0\0mount()\0unmount()\0"
    "mounted()\0unmounted()\0"
};

void Common::WebMounter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WebMounter *_t = static_cast<WebMounter *>(_o);
        switch (_id) {
        case 0: _t->mount(); break;
        case 1: _t->unmount(); break;
        case 2: _t->mounted(); break;
        case 3: _t->unmounted(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Common::WebMounter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Common::WebMounter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Common__WebMounter,
      qt_meta_data_Common__WebMounter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Common::WebMounter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Common::WebMounter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Common::WebMounter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Common__WebMounter))
        return static_cast<void*>(const_cast< WebMounter*>(this));
    return QObject::qt_metacast(_clname);
}

int Common::WebMounter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
